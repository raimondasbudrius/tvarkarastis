const express = require('express');
const cors = require('cors');
const axios = require('axios');

const app = express();
const PORT = 3000;

app.use(cors());

app.get('/api/schedule', async (req, res) => {
    try {
        const SHEET_ID = "2PACX-1vTH3SfQ4BkInzJNhJiZ_OxJ9QxCKdcQHHn-HPGg6UrZitqrx9fkNo42hjQZ808m4T2jiQYnNaBL0lNs";
        const GID = "1201117612";
        
        const url = `https://docs.google.com/spreadsheets/d/e/${SHEET_ID}/pub?tqx=out:json&gid=${GID}`;
        const response = await axios.get(url);
        res.json(response.data);
    } catch (error) {
        res.status(500).json({ error: error.message });
    }
});

app.listen(PORT, () => {
    console.log(`Proxy server running at http://localhost:${PORT}`);
});