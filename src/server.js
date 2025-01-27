import pkg from 'serialport';
const { SerialPort } = pkg;
import { ReadlineParser } from '@serialport/parser-readline';
import express, { json } from 'express';
import swaggerJsdoc from 'swagger-jsdoc';
import { serve, setup } from 'swagger-ui-express';
import { connect, Schema, model } from 'mongoose';
import axios from 'axios';
import readline from 'readline';
import { exec } from 'child_process';
import os from 'os';
import yamljs from 'yamljs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const app = express();
app.use(json());

let roomId = '';
let waiting = false;

// Define a schema and model for the data
const dataSchema = new Schema({
  roomId: String,
  temperature: Number,
  sound: Number,
  light: Number,
  score: Number,
  timestamp: { type: Date, default: Date.now }
});

const Data = model('Data', dataSchema);

const swaggerDocument = yamljs.load(path.join(__dirname, 'swagger.yaml'));

// Swagger UI setup
app.use('/api-docs', serve, setup(swaggerDocument));

// Function to compare scores
async function compare(score) {
  try {
    const allData = await Data.find({});
    const roomsWithDifferences = allData.map(data => ({
      roomId: data.roomId,
      score: data.score,
      difference: Math.abs(data.score - score)
    }));

    // Sort rooms by the difference in ascending order
    roomsWithDifferences.sort((a, b) => a.difference - b.difference);

    console.log(`SERVER : Received score: ${score}`);
    console.log('SERVER : Rooms sorted by score difference:', roomsWithDifferences);

    return {
      receivedScore: score,
      rooms: roomsWithDifferences
    };
  } catch (error) {
    console.error('SERVER : Error comparing scores:', error);
    throw error;
  }
}

// Getting preferences from node-red
app.post('/data', async (req, res) => {
  const { temperature, sound, light, score } = req.body;
  console.log(`SERVER : Temperature: ${temperature}, Sound: ${sound}, Light: ${light}, Score: ${score}`);

  if (waiting) {
    console.log('SERVER : Data received from Node-RED while waiting:', req.body);
    try {
      const allData = await Data.find({});
      const roomsWithDifferences = allData.map(data => ({
        roomId: data.roomId,
        temperature: data.temperature,
        sound: data.sound,
        light: data.light,
        score: data.score,
        difference: Math.abs(data.score - score)
      }));

      // Sort rooms by the difference in ascending order
      roomsWithDifferences.sort((a, b) => a.difference - b.difference);

      console.log(`SERVER : Received score: ${score}`);
      console.log('SERVER : Rooms sorted by score difference:', roomsWithDifferences);

      const comparisonResult = {
        receivedScore: score,
        rooms: roomsWithDifferences
      };

      // Send the comparison result to Node-RED
      await axios.post('http://localhost:1880/compare', comparisonResult);
      console.log('SERVER : Comparison result sent to Node-RED');
    } catch (error) {
      console.error('SERVER : Error during comparison:', error);
    }
  }

  res.send('SERVER : Data received successfully');
});

// Function to find the Arduino board
async function findArduino() {
  try {
    const ports = await SerialPort.list();
    const arduinoPort = ports.find(port => port.manufacturer && port.manufacturer.includes('Arduino'));
    if (arduinoPort) {
      console.log(`Arduino found on port: ${arduinoPort.path}`);
      return arduinoPort.path;
    } else {
      console.error('Arduino not found');
      return null;
    }
  } catch (error) {
    console.error(`Error finding Arduino: ${error.message}`);
    return null;
  }
}

// Connect to MongoDB
const dbURI = 'mongodb+srv://ddcd:ddcd@rooms.3och0.mongodb.net/?retryWrites=true&w=majority&appName=Rooms';

(async () => {
  connect(dbURI)
    .then(() => {
      console.log('MongoDB connected');

      const PORT = process.env.PORT || 3000;
      app.listen(PORT, () => {
        console.log(`SERVER : Server is running on port ${PORT}`);

        const rl = readline.createInterface({
          input: process.stdin,
          output: process.stdout
        });

        const askForChoice = () => {
          rl.question('SERVER : Choose an option:\n1. Register a new room\n2. Compare existing rooms\nEnter your choice: ', async (choice) => {
            if (choice === '1') {
              const portPath = await findArduino();
              if (!portPath) {
                askForChoice(); // Re-prompt if Arduino is not found
                return;
              }
              const arduinoPort = new SerialPort({ path: portPath, baudRate: 9600 });
              const parser = arduinoPort.pipe(new ReadlineParser({ delimiter: '\n' }));

              rl.question('SERVER : Please enter the room ID: ', (answer) => {
                roomId = answer;
                console.log(`SERVER : Room ID set to: ${roomId}`);
                arduinoPort.write(roomId + '\n');

                parser.on('data', async (data) => {
                  console.log(`SERVER : Received data from Arduino: ${data}`);
                  const [receivedRoomId, temperature, sound, light, score] = data.trim().split(',').map((value, index) => index === 0 ? value : Number(value));
                  
                  // Only save data if temperature, sound, and light are present
                  if (temperature !== undefined && sound !== undefined && light !== undefined) {
                    const newData = new Data({ roomId: receivedRoomId, temperature, sound, light, score });
                    try {
                      await newData.save();
                      console.log('SERVER : Data saved to MongoDB');
                      //Ask for the user's choice again
                      askForChoice();
                    } catch (error) {
                      console.error('SERVER : Error saving data to MongoDB (Missing data):', error);
                    }
                  }
                });
              });
            } else if (choice === '2') {
              console.log('SERVER : Launching server to compare preferecnes with registered rooms...');
              waiting = true;
              console.log('SERVER : Waiting for data from node-red...');
            } else {
              console.log('SERVER : Invalid choice. Exiting...');
              process.exit(1);
            }
          });
        };

        askForChoice();
      });
    })
    .catch(err => console.log(err));
})();


