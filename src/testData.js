const axios = require('axios');

const testData = {
  temperature: 22.5,
  sound: 57.0,
  light: 300.0
};

axios.post('http://localhost:1880/data', testData)
  .then(response => {
    console.log('Data sent successfully:', response.data);
  })
  .catch(error => {
    console.error('Error sending data:', error);
  });