import React, { useState, useEffect } from 'react';
import axios from 'axios';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import './App.css';

function App() {
  const [sensorData, setSensorData] = useState([]);
  const [latestData, setLatestData] = useState({});

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await axios.get('http://localhost:3000/api/sensor-data');
        setSensorData(response.data);
        setLatestData(response.data[0] || {});
      } catch (error) {
        console.error('Error fetching data:', error);
      }
    };

    fetchData();
    const interval = setInterval(fetchData, 60000); // Actualizar cada minuto

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="App">
      <h1>Environmental Monitoring Dashboard</h1>
      <div className="dashboard">
        <div className="metric-card">
          <div className="metric-title">Temperature</div>
          <div className="metric-value">{latestData.temperature?.toFixed(1)}°C</div>
        </div>
        <div className="metric-card">
          <div className="metric-title">Humidity</div>
          <div className="metric-value">{latestData.humidity?.toFixed(1)}%</div>
        </div>
        <div className="metric-card">
          <div className="metric-title">Air Quality</div>
          <div className="metric-value">{latestData.airQuality?.toFixed(1)} ppm</div>
        </div>
      </div>
      <div className="chart-container">
        <ResponsiveContainer width="100%" height={400}>
          <LineChart data={sensorData}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="timestamp" tickFormatter={(unixTime) => new Date(unixTime).toLocaleTimeString()} />
            <YAxis yAxisId="left" />
            <YAxis yAxisId="right" orientation="right" />
            <Tooltip labelFormatter={(unixTime) => new Date(unixTime).toLocaleString()} />
            <Legend />
            <Line yAxisId="left" type="monotone" dataKey="temperature" stroke="#8884d8" name="Temperature (°C)" />
            <Line yAxisId="left" type="monotone" dataKey="humidity" stroke="#82ca9d" name="Humidity (%)" />
            <Line yAxisId="right" type="monotone" dataKey="airQuality" stroke="#ffc658" name="Air Quality (ppm)" />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}

export default App;