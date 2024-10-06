const mqtt = require("mqtt");

const MQTT_SERVER = "mqtt://broker.hivemq.com";

async function connect() {
  const client = await mqtt.connectAsync(MQTT_SERVER);
  return client;
}

function random(min, max) {
  return Math.floor(Math.random() * (max - min) + min);
}

function sendLum(client) {
  const lum = random(1, 1000);
  client.publish("/sihs/equipe-gab-nando/1/lum", `${lum}`);
}

function sendTemp(client, ac) {
  const temp = random(1, 70);
  client.publish(`/sihs/equipe-gab-nando/${ac}/temp`, `${temp}`);
}

async function main() {
  const client = await connect();
  console.log("Client connected.");

  setInterval(() => sendLum(client), 1000);
  setInterval(() => sendTemp(client, 2), 1000);
  setInterval(() => sendTemp(client, 3), 1000);

  client.subscribe("/sihs/equipe-gab-nando/1/luz");
  client.subscribe("/sihs/equipe-gab-nando/2/ac");
  client.subscribe("/sihs/equipe-gab-nando/3/ac");

  client.on("message", (topic, payload) => {
    const content = String(payload);
    console.log(`[INFO] topic: ${topic} | payload: ${content}`);
  });
}

main();
