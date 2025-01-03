import { serve } from "bun";

const wavFile = "recorded_audio.wav";
const writer = Bun.file(wavFile).writer();

const writeWavHeader = (sampleRate = 8000, bitsPerSample = 16, channels = 1) => {
  const dataSize = 0;
  const buffer = new ArrayBuffer(44);
  const view = new DataView(buffer);
  
  view.setUint32(0, 0x52494646, false);
  view.setUint32(4, 36 + dataSize, true);
  view.setUint32(8, 0x57415645, false);
  view.setUint32(12, 0x666D7420, false);
  view.setUint32(16, 16, true);
  view.setUint16(20, 1, true);
  view.setUint16(22, channels, true);
  view.setUint32(24, sampleRate, true);
  view.setUint32(28, sampleRate * channels * (bitsPerSample / 8), true);
  view.setUint16(32, channels * (bitsPerSample / 8), true);
  view.setUint16(34, bitsPerSample, true);
  view.setUint32(36, 0x64617461, false);
  view.setUint32(40, dataSize, true);
  
  return new Uint8Array(buffer);
};

let totalBytes = 0;

const server = serve({
  port: 5454,
  fetch(req, server) {
    if (server.upgrade(req)) {
      return;
    }
    return new Response("WebSocket server running");
  },
  websocket: {
    open(ws) {
      console.log("Client connected");
      writer.write(writeWavHeader());
      totalBytes = 0;
    },
    message(ws, message) {
      if (message instanceof Uint8Array) {
        writer.write(message);
        totalBytes += message.length;
      }
    },
    close(ws) {
      console.log("Client disconnected");
      writer.flush();
      
      const file = Bun.file(wavFile);
      const contents = file.arrayBuffer();
      
      contents.then((buffer) => {
        const view = new DataView(buffer);
        view.setUint32(4, 36 + totalBytes, true);
        view.setUint32(40, totalBytes, true);
        
        Bun.write(wavFile, buffer);
        console.log(`WAV file saved with ${totalBytes} bytes of audio data`);
      });
    }
  },
});

console.log(`WebSocket server listening on port ${server.port}`);