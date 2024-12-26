const express = require('express');
const swaggerJsdoc = require('swagger-jsdoc');
const swaggerUi = require('swagger-ui-express');
const mongoose = require('mongoose');
const axios = require('axios');

const app = express();
app.use(express.json());

const swaggerOptions = {
  swaggerDefinition: {
    openapi: '3.0.0',
    info: {
      title: 'Arduino Data API',
      version: '1.0.0',
      description: 'API to receive data from Arduino',
    },
  },
  apis: ['./server.js'],
};

const swaggerDocs = swaggerJsdoc(swaggerOptions);
app.use('/api-docs', swaggerUi.serve, swaggerUi.setup(swaggerDocs));

/**
 * @swagger
 * /data:
 *   post:
 *     summary: Receive data from Arduino
 *     requestBody:
 *       required: true
 *       content:
 *         application/json:
 *           schema:
 *             type: object
 *             properties:
 *               temperature:
 *                 type: number
 *               sound:
 *                 type: number
 *               light:
 *                 type: number
 *     responses:
 *       200:
 *         description: Data received successfully
 */

app.post('/data', async (req, res) => {
  const { temperature, sound, light } = req.body;
  console.log(`Temperature: ${temperature}, Sound: ${sound}, Light: ${light}`);

  try {
    const response = await axios.post('http://localhost:1880/data', {
      temperature,
      sound,
      light
    });
    console.log('Data sent to Node-RED:', response.data);
  } catch (error) {
    console.error('Error sending data to Node-RED:', error);
  }

  res.send('Data received successfully');
});

// Connect to MongoDB
const dbURI = 'mongodb+srv://ddcd:ddcd@rooms.3och0.mongodb.net/?retryWrites=true&w=majority&appName=Rooms';
mongoose.connect(dbURI, { useNewUrlParser: true })
  .then(() => console.log('MongoDB connected'))
  .catch(err => console.log(err));


const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});