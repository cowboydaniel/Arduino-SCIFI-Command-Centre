#include <WiFiS3.h>
#include <pgmspace.h>

// WiFi credentials - UPDATE THESE
const char* ssid = "outbackhut";
const char* password = "wildmonkeys2810";

WiFiServer server(80);

const char PAGE_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Quantum Command Center</title><style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#000;color:#0f0;font-family:'Courier New',monospace;overflow:hidden;height:100vh}
.container{height:100vh;padding:20px;display:flex;flex-direction:column}
.header{text-align:center;border:2px solid #0f0;padding:15px;margin-bottom:12px;background:rgba(0,255,0,0.05);box-shadow:0 0 20px #0f0}
.header h1{font-size:2em;text-shadow:0 0 10px #0f0;animation:pulse 2s infinite}
.nav{display:flex;gap:12px;justify-content:center;flex-wrap:wrap;margin-bottom:16px}
.nav a{color:#0ff;text-decoration:none;border:1px solid #0ff;padding:6px 12px;background:rgba(0,255,255,0.08);transition:all 0.2s}
.nav a:hover{background:rgba(0,255,255,0.2);box-shadow:0 0 10px #0ff}
.systems{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:15px;flex:1;overflow-y:auto}
.system{border:2px solid #0f0;padding:15px;background:rgba(0,255,0,0.03);transition:all 0.3s}
.system:hover{background:rgba(0,255,0,0.1);box-shadow:0 0 15px #0f0}
.system h2{font-size:1.2em;margin-bottom:10px;color:#0ff;text-shadow:0 0 5px #0ff}
.readout{margin:8px 0;padding:8px;background:rgba(0,255,0,0.1);border-left:3px solid #0f0}
.value{color:#ff0;font-weight:bold;float:right}
.button{display:block;width:100%;margin-top:10px;padding:10px;background:#0a0;color:#000;border:2px solid #0f0;cursor:pointer;font-family:inherit;font-size:1em;font-weight:bold;transition:all 0.2s}
.button:hover{background:#0f0;box-shadow:0 0 20px #0f0;transform:scale(1.02)}
.button:active{transform:scale(0.98)}
.warning{color:#f00;animation:blink 1s infinite}
.gauge{height:20px;background:#111;border:1px solid #0f0;margin:10px 0;position:relative;overflow:hidden}
.gauge-fill{height:100%;background:linear-gradient(90deg,#0f0,#ff0,#f00);transition:width 0.5s}
.page{flex:1;border:2px solid #0f0;padding:20px;background:rgba(0,255,0,0.03);overflow-y:auto}
.page h2{color:#0ff;margin-bottom:12px}
.page ul{list-style:none}
.page li{padding:6px 0;border-bottom:1px solid rgba(0,255,0,0.2)}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.7}}
@keyframes blink{0%,100%{opacity:1}50%{opacity:0.3}}
.status{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:5px}
.status.active{background:#0f0;box-shadow:0 0 10px #0f0}
.status.inactive{background:#333}
</style></head><body><div class='container'><div class='header'>
<h1>⚡ QUANTUM COMMAND CENTER ⚡</h1>
<div>SYSTEM STATUS: <span style='color:#0f0'>OPERATIONAL</span> | THREAT LEVEL: <span style='color:#ff0'>MODERATE</span></div>
</div>
)rawliteral";

const char NAV_HTML[] PROGMEM = R"rawliteral(
<nav class='nav'>
  <a href='/'>Home</a>
  <a href='/systems'>Systems</a>
  <a href='/map'>Sector Map</a>
</nav>
)rawliteral";

const char INDEX_CONTENT[] PROGMEM = R"rawliteral(
<div class='systems'>
  <div class='system'><h2>⚛️ Quantum Flux Capacitor</h2>
  <div class='readout'>Flux State: <span class='value' id='flux'>STABLE</span></div>
  <div class='readout'>Particle Count: <span class='value' id='particles'>0</span></div>
  <div class='readout'>Entanglement: <span class='value' id='entangle'>0%</span></div>
  <div class='gauge'><div class='gauge-fill' id='flux-gauge' style='width:50%'></div></div>
  <button class='button' onclick='activateFlux()'>ACTIVATE FLUX</button></div>

  <div class='system'><h2>⏰ Temporal Navigation System</h2>
  <div class='readout'>Current Era: <span class='value' id='era'>2025 CE</span></div>
  <div class='readout'>Temporal Drift: <span class='value' id='drift'>±0 yrs</span></div>
  <div class='readout'>Causality Index: <span class='value' id='causality'>1.000</span></div>
  <button class='button' onclick='timeTravel()'>INITIATE TIME JUMP</button></div>

  <div class='system'><h2>🌀 Wormhole Generator</h2>
  <div class='readout'>Stability: <span class='value' id='stability'>0%</span></div>
  <div class='readout'>Aperture Size: <span class='value' id='aperture'>0 m</span></div>
  <div class='readout'>Destination: <span class='value' id='destination'>UNKNOWN</span></div>
  <div class='gauge'><div class='gauge-fill' id='wormhole-gauge' style='width:0%'></div></div>
  <button class='button' onclick='openWormhole()'>OPEN WORMHOLE</button></div>

  <div class='system'><h2>🕳️ Dark Matter Detector</h2>
  <div class='readout'>Density: <span class='value' id='dark-matter'>0 μg/cm³</span></div>
  <div class='readout'>Fluctuation: <span class='value' id='fluctuation'>MINIMAL</span></div>
  <div class='readout'><span class='status' id='dm-status'></span>Detection Grid: <span class='value' id='grid'>OFFLINE</span></div>
  <button class='button' onclick='scanDarkMatter()'>SCAN SECTOR</button></div>

  <div class='system'><h2>💥 Antimatter Containment</h2>
  <div class='readout'>Containment: <span class='value' id='containment'>100%</span></div>
  <div class='readout'>Antimatter: <span class='value' id='antimatter'>0.000 g</span></div>
  <div class='readout'>Field Strength: <span class='value' id='field'>MAXIMUM</span></div>
  <div class='gauge'><div class='gauge-fill' id='antimatter-gauge' style='width:100%'></div></div>
  <button class='button' onclick='generateAntimatter()'>GENERATE ANTIMATTER</button></div>

  <div class='system'><h2>🌌 Dimensional Phase Shifter</h2>
  <div class='readout'>Current Dimension: <span class='value' id='dimension'>3D</span></div>
  <div class='readout'>Phase Angle: <span class='value' id='phase'>0°</span></div>
  <div class='readout'>Reality Anchor: <span class='value' id='anchor'>LOCKED</span></div>
  <button class='button' onclick='shiftDimension()'>SHIFT PHASE</button></div>

  <div class='system'><h2>⚡ Tachyon Pulse Emitter</h2>
  <div class='readout'>Pulse Rate: <span class='value' id='pulse-rate'>0 Hz</span></div>
  <div class='readout'>FTL Factor: <span class='value' id='ftl'>0.0x</span></div>
  <div class='readout'>Energy Draw: <span class='value' id='energy'>0 GW</span></div>
  <button class='button' onclick='emitTachyon()'>EMIT PULSE</button></div>

  <div class='system'><h2>⚫ Singularity Simulator</h2>
  <div class='readout'>Schwarzschild Radius: <span class='value' id='radius'>0 km</span></div>
  <div class='readout'>Hawking Radiation: <span class='value' id='hawking'>0 K</span></div>
  <div class='readout'>Event Horizon: <span class='value' id='horizon'>STABLE</span></div>
  <div class='gauge'><div class='gauge-fill' id='singularity-gauge' style='width:0%'></div></div>
  <button class='button' onclick='createSingularity()'>CREATE SINGULARITY</button></div>
</div>
)rawliteral";

const char INDEX_SCRIPT[] PROGMEM = R"rawliteral(
<script>
function rand(min,max){return Math.floor(Math.random()*(max-min+1))+min}
function randFloat(min,max){return(Math.random()*(max-min)+min).toFixed(3)}

function activateFlux(){
let states=['STABLE','FLUCTUATING','CRITICAL','HARMONIZED'];
document.getElementById('flux').textContent=states[rand(0,3)];
document.getElementById('particles').textContent=rand(1000,9999);
document.getElementById('entangle').textContent=rand(0,100)+'%';
document.getElementById('flux-gauge').style.width=rand(20,100)+'%';}

function timeTravel(){
let eras=['2025 CE','1885 CE','2155 CE','3024 CE','476 CE','10000 BCE'];
document.getElementById('era').textContent=eras[rand(0,5)];
document.getElementById('drift').textContent='±'+rand(0,5000)+' yrs';
document.getElementById('causality').textContent=randFloat(0.5,1.5);}

function openWormhole(){
let dests=['ALPHA CENTAURI','ANDROMEDA','PARALLEL EARTH','VOID','PROXIMA B'];
document.getElementById('stability').textContent=rand(0,100)+'%';
document.getElementById('aperture').textContent=randFloat(0,100)+' m';
document.getElementById('destination').textContent=dests[rand(0,4)];
document.getElementById('wormhole-gauge').style.width=rand(0,100)+'%';}

function scanDarkMatter(){
let flucts=['MINIMAL','MODERATE','SEVERE','ANOMALOUS'];
document.getElementById('dark-matter').textContent=randFloat(0,10)+' μg/cm³';
document.getElementById('fluctuation').textContent=flucts[rand(0,3)];
let online=rand(0,1);
document.getElementById('dm-status').className='status '+(online?'active':'inactive');
document.getElementById('grid').textContent=online?'ONLINE':'OFFLINE';}

function generateAntimatter(){
let c=rand(85,100);
document.getElementById('containment').textContent=c+'%';
document.getElementById('antimatter').textContent=randFloat(0,1)+' g';
document.getElementById('field').textContent=c>95?'MAXIMUM':'DEGRADING';
document.getElementById('antimatter-gauge').style.width=c+'%';}

function shiftDimension(){
let dims=['3D','4D','5D','2D','11D','FRACTIONAL'];
document.getElementById('dimension').textContent=dims[rand(0,5)];
document.getElementById('phase').textContent=rand(0,360)+'°';
document.getElementById('anchor').textContent=rand(0,1)?'LOCKED':'DRIFTING';}

function emitTachyon(){
document.getElementById('pulse-rate').textContent=randFloat(0,1000)+' Hz';
document.getElementById('ftl').textContent=randFloat(0,10)+'x';
document.getElementById('energy').textContent=randFloat(0,999)+' GW';}

function createSingularity(){
document.getElementById('radius').textContent=randFloat(0,100)+' km';
document.getElementById('hawking').textContent=randFloat(0,1000)+' K';
document.getElementById('horizon').textContent=rand(0,1)?'STABLE':'COLLAPSING';
document.getElementById('singularity-gauge').style.width=rand(20,100)+'%';}

setInterval(()=>{
if(Math.random()>0.7)activateFlux();
if(Math.random()>0.8)scanDarkMatter();
},3000);
</script>
)rawliteral";

const char SYSTEMS_CONTENT[] PROGMEM = R"rawliteral(
<div class='page'>
  <h2>Systems Directory</h2>
  <p>Access the full list of quantum subsystems and diagnostic feeds below.</p>
  <ul>
    <li>Quantum Flux Capacitor - Stability calibrations and particle logs.</li>
    <li>Temporal Navigation - Drift corrections and causality checks.</li>
    <li>Wormhole Generator - Aperture metrics and destination routing.</li>
    <li>Dark Matter Detector - Density sweeps and fluctuation alerts.</li>
    <li>Antimatter Containment - Field strength and reserve status.</li>
    <li>Dimensional Phase Shifter - Phase alignment controls.</li>
    <li>Tachyon Pulse Emitter - Pulse harmonics and energy draw.</li>
    <li>Singularity Simulator - Event horizon stability tracking.</li>
  </ul>
</div>
)rawliteral";

const char MAP_CONTENT[] PROGMEM = R"rawliteral(
<div class='page'>
  <h2>Sector Map</h2>
  <p>Starmap telemetry uplink is active. Plotting navigation corridors and anomaly beacons.</p>
  <ul>
    <li>Sector A-19: Stable trade lanes online.</li>
    <li>Sector K-07: Temporal distortion warning.</li>
    <li>Sector Z-42: Wormhole signatures detected.</li>
    <li>Sector Q-11: Dark matter surge reported.</li>
  </ul>
</div>
)rawliteral";

const char NOT_FOUND_CONTENT[] PROGMEM = R"rawliteral(
<div class='page'>
  <h2>404 - Signal Lost</h2>
  <p>The requested console channel is offline or does not exist. Return to the bridge and reroute your query.</p>
</div>
)rawliteral";

const char PAGE_FOOTER[] PROGMEM = R"rawliteral(
</div></body></html>
)rawliteral";

String extractPath(const String& requestLine) {
  int firstSpace = requestLine.indexOf(' ');
  if (firstSpace == -1) {
    return "/";
  }
  int secondSpace = requestLine.indexOf(' ', firstSpace + 1);
  if (secondSpace == -1) {
    return "/";
  }
  String path = requestLine.substring(firstSpace + 1, secondSpace);
  int queryIndex = path.indexOf('?');
  if (queryIndex != -1) {
    path = path.substring(0, queryIndex);
  }
  return path;
}

void sendHtmlPage(WiFiClient& client, const char* status, const char* content, const char* script) {
  client.println(status);
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.print(FPSTR(PAGE_HEAD));
  client.print(FPSTR(NAV_HTML));
  client.print(FPSTR(content));
  if (script != nullptr) {
    client.print(FPSTR(script));
  }
  client.print(FPSTR(PAGE_FOOTER));
  client.println();
}

void servePage(const char* path, WiFiClient& client) {
  if (strcmp(path, "/") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", INDEX_CONTENT, INDEX_SCRIPT);
    return;
  }
  if (strcmp(path, "/systems") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", SYSTEMS_CONTENT, nullptr);
    return;
  }
  if (strcmp(path, "/map") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", MAP_CONTENT, nullptr);
    return;
  }
  sendHtmlPage(client, "HTTP/1.1 404 Not Found", NOT_FOUND_CONTENT, nullptr);
}

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
    String requestLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (c == '\n') {
          if (currentLine.length() == 0) {
            String path = extractPath(requestLine);
            servePage(path.c_str(), client);
            break;
          } else {
            if (requestLine.length() == 0) {
              requestLine = currentLine;
            }
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
