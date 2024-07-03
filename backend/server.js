const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const firebase = require('firebase/app');
require('firebase/database');
require('dotenv').config();

const app = express();
app.use(cors());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// ... (resto del código del servidor)

// Añadir una ruta GET para obtener los datos
app.get('/api/sensor-data', (req, res) => {
  database.ref('sensor-data').orderByChild('timestamp').limitToLast(10).once('value', (snapshot) => {
    const data = [];
    snapshot.forEach((childSnapshot) => {
      data.push({
        id: childSnapshot.key,
        ...childSnapshot.val()
      });
    });
    res.json(data.reverse());
  }, (error) => {
    res.status(500).send('Error retrieving data');
  });
});

// ... (resto del código del servidor)