#include <WiFiS3.h>

// WiFi credentials - UPDATE THESE
const char* ssid = "outbackhut";
const char* password = "wildmonkeys2810";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Initializing Quantum Systems...");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  delay(1000); // Give it a second to get IP
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Navigate to this IP in your browser for FULL SCREEN experience");
  
  server.begin();
  Serial.println("Quantum Command Center Online");
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New connection");
    String currentLine = "";
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Send the HTML page
            client.println("<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Quantum Command Center</title><style>");
            client.println("*{margin:0;padding:0;box-sizing:border-box}");
            client.println("body{background:#000;color:#0f0;font-family:'Courier New',monospace;overflow:hidden;height:100vh}");
            client.println(".container{height:100vh;padding:20px;display:flex;flex-direction:column}");
            client.println(".header{text-align:center;border:2px solid #0f0;padding:15px;margin-bottom:20px;background:rgba(0,255,0,0.05);box-shadow:0 0 20px #0f0}");
            client.println(".header h1{font-size:2em;text-shadow:0 0 10px #0f0;animation:pulse 2s infinite}");
            client.println(".systems{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:15px;flex:1;overflow-y:auto}");
            client.println(".system{border:2px solid #0f0;padding:15px;background:rgba(0,255,0,0.03);transition:all 0.3s}");
            client.println(".system:hover{background:rgba(0,255,0,0.1);box-shadow:0 0 15px #0f0}");
            client.println(".system h2{font-size:1.2em;margin-bottom:10px;color:#0ff;text-shadow:0 0 5px #0ff}");
            client.println(".readout{margin:8px 0;padding:8px;background:rgba(0,255,0,0.1);border-left:3px solid #0f0}");
            client.println(".value{color:#ff0;font-weight:bold;float:right}");
            client.println(".button{display:block;width:100%;margin-top:10px;padding:10px;background:#0a0;color:#000;border:2px solid #0f0;cursor:pointer;font-family:inherit;font-size:1em;font-weight:bold;transition:all 0.2s}");
            client.println(".button:hover{background:#0f0;box-shadow:0 0 20px #0f0;transform:scale(1.02)}");
            client.println(".button:active{transform:scale(0.98)}");
            client.println(".warning{color:#f00;animation:blink 1s infinite}");
            client.println(".gauge{height:20px;background:#111;border:1px solid #0f0;margin:10px 0;position:relative;overflow:hidden}");
            client.println(".gauge-fill{height:100%;background:linear-gradient(90deg,#0f0,#ff0,#f00);transition:width 0.5s}");
            client.println("@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.7}}");
            client.println("@keyframes blink{0%,100%{opacity:1}50%{opacity:0.3}}");
            client.println(".status{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:5px}");
            client.println(".status.active{background:#0f0;box-shadow:0 0 10px #0f0}");
            client.println(".status.inactive{background:#333}");
            client.println("</style></head><body><div class='container'><div class='header'>");
            client.println("<h1>⚡ QUANTUM COMMAND CENTER ⚡</h1>");
            client.println("<div>SYSTEM STATUS: <span style='color:#0f0'>OPERATIONAL</span> | THREAT LEVEL: <span style='color:#ff0'>MODERATE</span></div>");
            client.println("</div><div class='systems'>");
            
            // Quantum Flux Capacitor
            client.println("<div class='system'><h2>⚛️ Quantum Flux Capacitor</h2>");
            client.println("<div class='readout'>Flux State: <span class='value' id='flux'>STABLE</span></div>");
            client.println("<div class='readout'>Particle Count: <span class='value' id='particles'>0</span></div>");
            client.println("<div class='readout'>Entanglement: <span class='value' id='entangle'>0%</span></div>");
            client.println("<div class='gauge'><div class='gauge-fill' id='flux-gauge' style='width:50%'></div></div>");
            client.println("<button class='button' onclick='activateFlux()'>ACTIVATE FLUX</button></div>");
            
            // Temporal Navigator
            client.println("<div class='system'><h2>⏰ Temporal Navigation System</h2>");
            client.println("<div class='readout'>Current Era: <span class='value' id='era'>2025 CE</span></div>");
            client.println("<div class='readout'>Temporal Drift: <span class='value' id='drift'>±0 yrs</span></div>");
            client.println("<div class='readout'>Causality Index: <span class='value' id='causality'>1.000</span></div>");
            client.println("<button class='button' onclick='timeTravel()'>INITIATE TIME JUMP</button></div>");
            
            // Wormhole Generator
            client.println("<div class='system'><h2>🌀 Wormhole Generator</h2>");
            client.println("<div class='readout'>Stability: <span class='value' id='stability'>0%</span></div>");
            client.println("<div class='readout'>Aperture Size: <span class='value' id='aperture'>0 m</span></div>");
            client.println("<div class='readout'>Destination: <span class='value' id='destination'>UNKNOWN</span></div>");
            client.println("<div class='gauge'><div class='gauge-fill' id='wormhole-gauge' style='width:0%'></div></div>");
            client.println("<button class='button' onclick='openWormhole()'>OPEN WORMHOLE</button></div>");
            
            // Dark Matter Detector
            client.println("<div class='system'><h2>🕳️ Dark Matter Detector</h2>");
            client.println("<div class='readout'>Density: <span class='value' id='dark-matter'>0 μg/cm³</span></div>");
            client.println("<div class='readout'>Fluctuation: <span class='value' id='fluctuation'>MINIMAL</span></div>");
            client.println("<div class='readout'><span class='status' id='dm-status'></span>Detection Grid: <span class='value' id='grid'>OFFLINE</span></div>");
            client.println("<button class='button' onclick='scanDarkMatter()'>SCAN SECTOR</button></div>");
            
            // Antimatter Containment
            client.println("<div class='system'><h2>💥 Antimatter Containment</h2>");
            client.println("<div class='readout'>Containment: <span class='value' id='containment'>100%</span></div>");
            client.println("<div class='readout'>Antimatter: <span class='value' id='antimatter'>0.000 g</span></div>");
            client.println("<div class='readout'>Field Strength: <span class='value' id='field'>MAXIMUM</span></div>");
            client.println("<div class='gauge'><div class='gauge-fill' id='antimatter-gauge' style='width:100%'></div></div>");
            client.println("<button class='button' onclick='generateAntimatter()'>GENERATE ANTIMATTER</button></div>");
            
            // Dimensional Phase Shifter
            client.println("<div class='system'><h2>🌌 Dimensional Phase Shifter</h2>");
            client.println("<div class='readout'>Current Dimension: <span class='value' id='dimension'>3D</span></div>");
            client.println("<div class='readout'>Phase Angle: <span class='value' id='phase'>0°</span></div>");
            client.println("<div class='readout'>Reality Anchor: <span class='value' id='anchor'>LOCKED</span></div>");
            client.println("<button class='button' onclick='shiftDimension()'>SHIFT PHASE</button></div>");
            
            // Tachyon Pulse Emitter
            client.println("<div class='system'><h2>⚡ Tachyon Pulse Emitter</h2>");
            client.println("<div class='readout'>Pulse Rate: <span class='value' id='pulse-rate'>0 Hz</span></div>");
            client.println("<div class='readout'>FTL Factor: <span class='value' id='ftl'>0.0x</span></div>");
            client.println("<div class='readout'>Energy Draw: <span class='value' id='energy'>0 GW</span></div>");
            client.println("<button class='button' onclick='emitTachyon()'>EMIT PULSE</button></div>");
            
            // Singularity Simulator
            client.println("<div class='system'><h2>⚫ Singularity Simulator</h2>");
            client.println("<div class='readout'>Schwarzschild Radius: <span class='value' id='radius'>0 km</span></div>");
            client.println("<div class='readout'>Hawking Radiation: <span class='value' id='hawking'>0 K</span></div>");
            client.println("<div class='readout'>Event Horizon: <span class='value' id='horizon'>STABLE</span></div>");
            client.println("<div class='gauge'><div class='gauge-fill' id='singularity-gauge' style='width:0%'></div></div>");
            client.println("<button class='button' onclick='createSingularity()'>CREATE SINGULARITY</button></div>");
            
            client.println("</div></div>");
            
            // JavaScript
            client.println("<script>");
            client.println("function rand(min,max){return Math.floor(Math.random()*(max-min+1))+min}");
            client.println("function randFloat(min,max){return(Math.random()*(max-min)+min).toFixed(3)}");
            
            client.println("function activateFlux(){");
            client.println("let states=['STABLE','FLUCTUATING','CRITICAL','HARMONIZED'];");
            client.println("document.getElementById('flux').textContent=states[rand(0,3)];");
            client.println("document.getElementById('particles').textContent=rand(1000,9999);");
            client.println("document.getElementById('entangle').textContent=rand(0,100)+'%';");
            client.println("document.getElementById('flux-gauge').style.width=rand(20,100)+'%';}");
            
            client.println("function timeTravel(){");
            client.println("let eras=['2025 CE','1885 CE','2155 CE','3024 CE','476 CE','10000 BCE'];");
            client.println("document.getElementById('era').textContent=eras[rand(0,5)];");
            client.println("document.getElementById('drift').textContent='±'+rand(0,5000)+' yrs';");
            client.println("document.getElementById('causality').textContent=randFloat(0.5,1.5);}");
            
            client.println("function openWormhole(){");
            client.println("let dests=['ALPHA CENTAURI','ANDROMEDA','PARALLEL EARTH','VOID','PROXIMA B'];");
            client.println("document.getElementById('stability').textContent=rand(0,100)+'%';");
            client.println("document.getElementById('aperture').textContent=randFloat(0,100)+' m';");
            client.println("document.getElementById('destination').textContent=dests[rand(0,4)];");
            client.println("document.getElementById('wormhole-gauge').style.width=rand(0,100)+'%';}");
            
            client.println("function scanDarkMatter(){");
            client.println("let flucts=['MINIMAL','MODERATE','SEVERE','ANOMALOUS'];");
            client.println("document.getElementById('dark-matter').textContent=randFloat(0,10)+' μg/cm³';");
            client.println("document.getElementById('fluctuation').textContent=flucts[rand(0,3)];");
            client.println("let online=rand(0,1);");
            client.println("document.getElementById('dm-status').className='status '+(online?'active':'inactive');");
            client.println("document.getElementById('grid').textContent=online?'ONLINE':'OFFLINE';}");
            
            client.println("function generateAntimatter(){");
            client.println("let c=rand(85,100);");
            client.println("document.getElementById('containment').textContent=c+'%';");
            client.println("document.getElementById('antimatter').textContent=randFloat(0,1)+' g';");
            client.println("document.getElementById('field').textContent=c>95?'MAXIMUM':'DEGRADING';");
            client.println("document.getElementById('antimatter-gauge').style.width=c+'%';}");
            
            client.println("function shiftDimension(){");
            client.println("let dims=['3D','4D','5D','2D','11D','FRACTIONAL'];");
            client.println("document.getElementById('dimension').textContent=dims[rand(0,5)];");
            client.println("document.getElementById('phase').textContent=rand(0,360)+'°';");
            client.println("document.getElementById('anchor').textContent=rand(0,1)?'LOCKED':'DRIFTING';}");
            
            client.println("function emitTachyon(){");
            client.println("document.getElementById('pulse-rate').textContent=randFloat(0,1000)+' Hz';");
            client.println("document.getElementById('ftl').textContent=randFloat(0,10)+'x';");
            client.println("document.getElementById('energy').textContent=randFloat(0,999)+' GW';}");
            
            client.println("function createSingularity(){");
            client.println("document.getElementById('radius').textContent=randFloat(0,100)+' km';");
            client.println("document.getElementById('hawking').textContent=randFloat(0,1000)+' K';");
            client.println("document.getElementById('horizon').textContent=rand(0,1)?'STABLE':'COLLAPSING';");
            client.println("document.getElementById('singularity-gauge').style.width=rand(20,100)+'%';}");
            
            client.println("setInterval(()=>{");
            client.println("if(Math.random()>0.7)activateFlux();");
            client.println("if(Math.random()>0.8)scanDarkMatter();");
            client.println("},3000);");
            client.println("</script></body></html>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Connection closed");
  }
}
