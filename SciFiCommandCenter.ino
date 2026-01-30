#include <Arduino.h>
#include <WiFiS3.h>

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#else
#ifndef PROGMEM
#define PROGMEM
#endif
#ifndef FPSTR
#define FPSTR(pstr_pointer) (pstr_pointer)
#endif
#endif

// WiFi credentials - UPDATE THESE
const char* ssid = "outbackhut";
const char* password = "wildmonkeys2810";

WiFiServer server(80);

const char PAGE_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Quantum Command Center</title><link rel='stylesheet' href='/styles.css'></head><body><div class='container'>
)rawliteral";

const char HEADER_HTML[] PROGMEM = R"rawliteral(
<header class='header'>
  <h1>⚡ QUANTUM COMMAND CENTER ⚡</h1>
  <div>SYSTEM STATUS: <span style='color:#0f0'>OPERATIONAL</span> | THREAT LEVEL: <span style='color:#ff0'>MODERATE</span></div>
</header>
)rawliteral";

const char NAV_HTML[] PROGMEM = R"rawliteral(
<nav class='nav'>
  <a href='/'>Home</a>
  <a href='/systems'>Systems</a>
  <a href='/map'>Sector Map</a>
  <a href='/telemetry'>Diagnostics</a>
</nav>
)rawliteral";

const char PAGE_FOOTER[] PROGMEM = R"rawliteral(
<footer class='footer'>
  <span>Quantum Command Center &mdash; All systems synchronized.</span>
</footer>
</div><script src='/app.js' defer></script></body></html>
)rawliteral";

const char STYLESHEET[] PROGMEM = R"rawliteral(
*{margin:0;padding:0;box-sizing:border-box}
body{background:#000;color:#0f0;font-family:'Courier New',monospace;overflow:hidden;height:100vh;position:relative}
body::before{content:"";position:fixed;inset:-50%;background:radial-gradient(circle at 20% 20%,rgba(0,255,255,0.25) 0,transparent 40%),radial-gradient(circle at 80% 30%,rgba(0,255,120,0.2) 0,transparent 45%),radial-gradient(circle at 40% 80%,rgba(0,120,255,0.2) 0,transparent 50%);animation:starDrift 40s linear infinite;opacity:0.35;z-index:0}
body::after{content:"";position:fixed;inset:0;background:linear-gradient(to bottom,rgba(0,255,255,0.05) 1px,transparent 1px);background-size:100% 4px;mix-blend-mode:screen;animation:scanLines 6s linear infinite;opacity:0.2;pointer-events:none;z-index:1}
.container{height:100vh;padding:20px;display:flex;flex-direction:column;position:relative;z-index:2}
.header{text-align:center;border:2px solid #0f0;padding:15px;margin-bottom:12px;background:rgba(0,255,0,0.05);box-shadow:0 0 20px #0f0;position:relative;overflow:hidden}
.header h1{font-size:2em;text-shadow:0 0 10px #0f0;animation:pulse 2s infinite}
.nav{display:flex;gap:12px;justify-content:center;flex-wrap:wrap;margin-bottom:16px}
.nav a{color:#0ff;text-decoration:none;border:1px solid #0ff;padding:6px 12px;background:rgba(0,255,255,0.08);transition:all 0.2s}
.nav a:hover{background:rgba(0,255,255,0.2);box-shadow:0 0 10px #0ff;transform:translateY(-1px)}
.systems{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:15px;flex:1;overflow-y:auto}
.system{border:2px solid #0f0;padding:15px;background:rgba(0,255,0,0.03);transition:transform 0.3s,box-shadow 0.3s,background 0.3s;position:relative;overflow:hidden}
.system::after{content:"";position:absolute;top:-60%;left:-20%;width:60%;height:200%;background:linear-gradient(120deg,transparent,rgba(0,255,255,0.12),transparent);animation:shimmer 6s linear infinite;opacity:0.6}
.system:hover{background:rgba(0,255,0,0.12);box-shadow:0 0 18px rgba(0,255,0,0.7);transform:translateY(-3px) scale(1.01)}
.system h2{font-size:1.2em;margin-bottom:10px;color:#0ff;text-shadow:0 0 5px #0ff}
.readout{margin:8px 0;padding:8px;background:rgba(0,255,0,0.1);border-left:3px solid #0f0;transition:transform 0.3s,box-shadow 0.3s}
.readout:hover{transform:translateX(4px);box-shadow:0 0 10px rgba(0,255,0,0.4)}
.value{color:#ff0;font-weight:bold;float:right;transition:color 0.3s,text-shadow 0.3s}
.button{display:block;width:100%;margin-top:10px;padding:10px;background:#0a0;color:#000;border:2px solid #0f0;cursor:pointer;font-family:inherit;font-size:1em;font-weight:bold;transition:all 0.2s}
.button:hover{background:#0f0;box-shadow:0 0 20px #0f0;transform:scale(1.02)}
.button:active{transform:scale(0.98)}
.warning{color:#f00;animation:blink 1s infinite}
.gauge{height:20px;background:#111;border:1px solid #0f0;margin:10px 0;position:relative;overflow:hidden}
.gauge::after{content:"";position:absolute;top:0;left:-40%;width:40%;height:100%;background:linear-gradient(90deg,transparent,rgba(255,255,255,0.3),transparent);animation:gaugeScan 2.5s linear infinite}
.gauge-fill{height:100%;background:linear-gradient(90deg,#0f0,#ff0,#f00);transition:width 0.8s ease,filter 0.3s}
.system:hover .gauge-fill{filter:brightness(1.2)}
.page{flex:1;border:2px solid #0f0;padding:20px;background:rgba(0,255,0,0.03);overflow-y:auto;position:relative;transition:transform 0.3s,box-shadow 0.3s}
.page:hover{transform:translateY(-2px);box-shadow:0 0 15px rgba(0,255,0,0.4)}
.page h2{color:#0ff;margin-bottom:12px}
.page ul{list-style:none}
.page li{padding:6px 0;border-bottom:1px solid rgba(0,255,0,0.2)}
.footer{margin-top:14px;border:1px solid rgba(0,255,255,0.4);padding:10px;text-align:center;color:#9ff;background:rgba(0,255,255,0.05);text-shadow:0 0 8px rgba(0,255,255,0.4)}
.telemetry-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));gap:16px;margin-top:12px}
.telemetry-panel{border:1px solid rgba(0,255,255,0.5);padding:12px;background:rgba(0,255,255,0.08);position:relative;overflow:hidden;transition:transform 0.3s,box-shadow 0.3s}
.telemetry-panel::after{content:"";position:absolute;top:-70%;left:-30%;width:60%;height:220%;background:linear-gradient(120deg,transparent,rgba(0,255,255,0.15),transparent);animation:shimmer 8s linear infinite}
.telemetry-panel:hover{transform:translateY(-3px) scale(1.01);box-shadow:0 0 18px rgba(0,255,255,0.6)}
.telemetry-panel h3{color:#0ff;margin-bottom:10px;text-shadow:0 0 6px rgba(0,255,255,0.6)}
#telemetry-chart{width:100%;height:220px;border:1px solid rgba(0,255,255,0.5);background:rgba(0,0,0,0.6);display:block}
#vector-chart{width:100%;height:140px;border:1px solid rgba(0,255,255,0.5);background:rgba(0,0,0,0.6)}
#vector-line{fill:none;stroke:#0ff;stroke-width:2;filter:drop-shadow(0 0 6px rgba(0,255,255,0.8));animation:vectorPulse 3s ease-in-out infinite}
.telemetry-legend{display:flex;justify-content:space-between;color:#9ff;font-size:0.8em;margin-top:6px}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.7}}
@keyframes blink{0%,100%{opacity:1}50%{opacity:0.3}}
@keyframes starDrift{0%{transform:translate3d(0,0,0)}100%{transform:translate3d(8%,12%,0)}}
@keyframes scanLines{0%{background-position:0 0}100%{background-position:0 120px}}
@keyframes shimmer{0%{transform:translateX(-120%)}100%{transform:translateX(220%)}}
@keyframes gaugeScan{0%{transform:translateX(-120%)}100%{transform:translateX(320%)}}
@keyframes vectorPulse{0%,100%{opacity:0.7}50%{opacity:1}}
.status{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:5px}
.status.active{background:#0f0;box-shadow:0 0 10px #0f0}
.status.inactive{background:#333}
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

const char TELEMETRY_CONTENT[] PROGMEM = R"rawliteral(
<div class='page telemetry'>
  <h2>Diagnostics & Telemetry</h2>
  <p>Streaming quantum diagnostics, spectral traces, and vector stability feedback.</p>
  <div class='telemetry-grid'>
    <div class='telemetry-panel'>
      <h3>Signal Spectrum</h3>
      <canvas id='telemetry-chart' width='600' height='240'></canvas>
    </div>
    <div class='telemetry-panel'>
      <h3>Core Readouts</h3>
      <div class='readout'>Reactor Load: <span class='value' id='diag-reactor'>72%</span></div>
      <div class='readout'>Comms Integrity: <span class='value' id='diag-comms'>98%</span></div>
      <div class='readout'>Shield Harmonics: <span class='value' id='diag-shield'>3.2 THz</span></div>
      <div class='readout'>Anomaly Index: <span class='value' id='diag-anomaly'>0.02</span></div>
      <div class='gauge'><div class='gauge-fill' id='diag-reactor-gauge' style='width:72%'></div></div>
    </div>
    <div class='telemetry-panel'>
      <h3>Vector Diagnostics</h3>
      <svg id='vector-chart' viewBox='0 0 200 120' preserveAspectRatio='none'>
        <polyline id='vector-line' points='0,60 40,40 80,70 120,30 160,80 200,50' />
      </svg>
      <div class='telemetry-legend'>
        <span>Nominal</span>
        <span>Variance</span>
        <span>Drift</span>
      </div>
    </div>
  </div>
</div>
)rawliteral";

const char APP_JS[] PROGMEM = R"rawliteral(
function rand(min,max){return Math.floor(Math.random()*(max-min+1))+min}
function randFloat(min,max){return Math.random()*(max-min)+min}
function easeInOutCubic(t){return t<0.5?4*t*t*t:1-Math.pow(-2*t+2,3)/2}
function parseNumber(text){const num=text.replace(/[^0-9.+-]/g,'');return parseFloat(num)||0}
function animateValue(id,target,options){
  const el=document.getElementById(id);
  if(!el){return;}
  const start=parseFloat(el.dataset.value||parseNumber(el.textContent));
  const startTime=performance.now();
  const duration=(options&&options.duration)||900;
  const decimals=(options&&options.decimals)!=null?options.decimals:0;
  const prefix=(options&&options.prefix)||'';
  const suffix=(options&&options.suffix)||'';
  const formatter=options&&options.formatter;
  function step(now){
    const progress=Math.min((now-startTime)/duration,1);
    const value=start+(target-start)*easeInOutCubic(progress);
    el.dataset.value=value;
    el.textContent=formatter?formatter(value):`${prefix}${value.toFixed(decimals)}${suffix}`;
    if(progress<1){requestAnimationFrame(step);}
  }
  requestAnimationFrame(step);
}

async function sendCommand(endpoint){
  try{
    const response=await fetch(endpoint);
    if(!response.ok){throw new Error(`Request failed: ${response.status}`);}
    return await response.json();
  }catch(error){
    console.warn('Command failed',endpoint,error);
    return null;
  }
}

async function activateFlux(){
  const data=await sendCommand('/api/flux');
  if(!data){return;}
  const flux=document.getElementById('flux');
  if(!flux){return;}
  flux.textContent=data.fluxState;
  animateValue('particles',data.particleCount,{decimals:0});
  animateValue('entangle',data.entangle,{decimals:0,suffix:'%'});
  const gauge=document.getElementById('flux-gauge');
  if(gauge){gauge.style.width=data.gauge+'%';}
}

async function timeTravel(){
  const data=await sendCommand('/api/time');
  if(!data){return;}
  const era=document.getElementById('era');
  if(!era){return;}
  era.textContent=data.era;
  animateValue('drift',data.drift,{decimals:0,prefix:'±',suffix:' yrs'});
  animateValue('causality',data.causality,{decimals:3});
}

async function openWormhole(){
  const data=await sendCommand('/api/wormhole');
  if(!data){return;}
  const destination=document.getElementById('destination');
  if(!destination){return;}
  animateValue('stability',data.stability,{decimals:0,suffix:'%'});
  animateValue('aperture',data.aperture,{decimals:2,suffix:' m'});
  destination.textContent=data.destination;
  const gauge=document.getElementById('wormhole-gauge');
  if(gauge){gauge.style.width=data.gauge+'%';}
}

async function scanDarkMatter(){
  const data=await sendCommand('/api/dark-matter');
  if(!data){return;}
  const fluctuation=document.getElementById('fluctuation');
  if(!fluctuation){return;}
  animateValue('dark-matter',data.density,{decimals:3,suffix:' μg/cm³'});
  fluctuation.textContent=data.fluctuation;
  const status=document.getElementById('dm-status');
  if(status){status.className='status '+(data.gridOnline?'active':'inactive');}
  const grid=document.getElementById('grid');
  if(grid){grid.textContent=data.gridOnline?'ONLINE':'OFFLINE';}
}

async function generateAntimatter(){
  const data=await sendCommand('/api/antimatter');
  if(!data){return;}
  const containment=document.getElementById('containment');
  if(!containment){return;}
  animateValue('containment',data.containment,{decimals:0,suffix:'%'});
  animateValue('antimatter',data.antimatter,{decimals:3,suffix:' g'});
  const field=document.getElementById('field');
  if(field){field.textContent=data.field;}
  const gauge=document.getElementById('antimatter-gauge');
  if(gauge){gauge.style.width=data.gauge+'%';}
}

async function shiftDimension(){
  const data=await sendCommand('/api/dimension');
  if(!data){return;}
  const dimension=document.getElementById('dimension');
  if(!dimension){return;}
  dimension.textContent=data.dimension;
  animateValue('phase',data.phase,{decimals:0,suffix:'°'});
  const anchor=document.getElementById('anchor');
  if(anchor){anchor.textContent=data.anchor;}
}

async function emitTachyon(){
  const data=await sendCommand('/api/tachyon');
  if(!data){return;}
  const pulse=document.getElementById('pulse-rate');
  if(!pulse){return;}
  animateValue('pulse-rate',data.pulseRate,{decimals:1,suffix:' Hz'});
  animateValue('ftl',data.ftl,{decimals:2,suffix:'x'});
  animateValue('energy',data.energy,{decimals:1,suffix:' GW'});
}

async function createSingularity(){
  const data=await sendCommand('/api/singularity');
  if(!data){return;}
  const radius=document.getElementById('radius');
  if(!radius){return;}
  animateValue('radius',data.radius,{decimals:2,suffix:' km'});
  animateValue('hawking',data.hawking,{decimals:1,suffix:' K'});
  const horizon=document.getElementById('horizon');
  if(horizon){horizon.textContent=data.horizon;}
  const gauge=document.getElementById('singularity-gauge');
  if(gauge){gauge.style.width=data.gauge+'%';}
}

function initHome(){
  if(!document.getElementById('flux')){return;}
  setInterval(()=>{
    if(Math.random()>0.65)activateFlux();
    if(Math.random()>0.7)timeTravel();
    if(Math.random()>0.7)openWormhole();
    if(Math.random()>0.8)scanDarkMatter();
    if(Math.random()>0.6)generateAntimatter();
    if(Math.random()>0.65)shiftDimension();
    if(Math.random()>0.6)emitTachyon();
    if(Math.random()>0.7)createSingularity();
  },2800);
}

function initTelemetry(){
  const canvas=document.getElementById('telemetry-chart');
  if(!canvas){return;}
  const ctx=canvas.getContext('2d');
  let telemetryData=new Array(48).fill(0).map(()=>randFloat(30,80));
  let chartDpr=1;

  function resizeCanvas(){
    const rect=canvas.getBoundingClientRect();
    chartDpr=window.devicePixelRatio||1;
    canvas.width=rect.width*chartDpr;
    canvas.height=rect.height*chartDpr;
    ctx.setTransform(1,0,0,1,0,0);
    ctx.scale(chartDpr,chartDpr);
  }

  function updateTelemetry(){
    telemetryData.push(randFloat(30,90));
    telemetryData.shift();
    const reactor=rand(60,95);
    animateValue('diag-reactor',reactor,{decimals:0,suffix:'%'});
    animateValue('diag-comms',rand(85,100),{decimals:0,suffix:'%'});
    animateValue('diag-shield',randFloat(2.5,4.5),{decimals:2,suffix:' THz'});
    animateValue('diag-anomaly',randFloat(0.01,0.12),{decimals:2});
    const gauge=document.getElementById('diag-reactor-gauge');
    if(gauge){gauge.style.width=reactor+'%';}
  }

  function drawChart(){
    ctx.save();
    ctx.setTransform(1,0,0,1,0,0);
    ctx.clearRect(0,0,canvas.width,canvas.height);
    ctx.restore();
    const width=canvas.clientWidth;
    const height=canvas.clientHeight;
    ctx.strokeStyle='rgba(0,255,255,0.2)';
    ctx.lineWidth=1;
    for(let i=0;i<5;i++){
      const y=(height/4)*i;
      ctx.beginPath();
      ctx.moveTo(0,y);
      ctx.lineTo(width,y);
      ctx.stroke();
    }
    ctx.strokeStyle='#0ff';
    ctx.lineWidth=2;
    ctx.beginPath();
    telemetryData.forEach((val,index)=>{
      const x=(width/(telemetryData.length-1))*index;
      const y=height-(val/100)*height;
      if(index===0){ctx.moveTo(x,y);}else{ctx.lineTo(x,y);}
    });
    ctx.stroke();
    requestAnimationFrame(drawChart);
  }

  function updateVector(){
    const line=document.getElementById('vector-line');
    if(!line){return;}
    const points=[];
    for(let i=0;i<6;i++){
      const x=i*40;
      const y=rand(20,100);
      points.push(`${x},${y}`);
    }
    line.setAttribute('points',points.join(' '));
  }

  window.addEventListener('resize',()=>{resizeCanvas();});
  resizeCanvas();
  updateTelemetry();
  drawChart();
  updateVector();
  setInterval(updateTelemetry,1400);
  setInterval(updateVector,1600);
}

window.addEventListener('DOMContentLoaded',()=>{
  initHome();
  initTelemetry();
});
)rawliteral";

const char NOT_FOUND_CONTENT[] PROGMEM = R"rawliteral(
<div class='page'>
  <h2>404 - Signal Lost</h2>
  <p>The requested console channel is offline or does not exist. Return to the bridge and reroute your query.</p>
</div>
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

void sendHtmlPage(WiFiClient& client, const char* status, const char* content) {
  client.println(status);
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.print(FPSTR(PAGE_HEAD));
  client.print(FPSTR(HEADER_HTML));
  client.print(FPSTR(NAV_HTML));
  client.print(FPSTR(content));
  client.print(FPSTR(PAGE_FOOTER));
  client.println();
}

void sendStylesheet(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/css");
  client.println("Cache-Control: max-age=86400");
  client.println("Connection: close");
  client.println();
  client.print(FPSTR(STYLESHEET));
  client.println();
}

void sendAppScript(WiFiClient& client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/javascript");
  client.println("Cache-Control: max-age=86400");
  client.println("Connection: close");
  client.println();
  client.print(FPSTR(APP_JS));
  client.println();
}

void sendJson(WiFiClient& client, const String& payload, const char* status = "HTTP/1.1 200 OK") {
  client.println(status);
  client.println("Content-type:application/json");
  client.println("Cache-Control: no-store");
  client.println("Connection: close");
  client.println();
  client.print(payload);
  client.println();
}

const int PIN_FLUX = 2;
const int PIN_TIME = 3;
const int PIN_WORMHOLE = 4;
const int PIN_DARK_MATTER = 5;
const int PIN_ANTIMATTER = 6;
const int PIN_DIMENSION = 7;
const int PIN_TACHYON = 8;
const int PIN_SINGULARITY = 9;

bool fluxActive = false;
bool timeActive = false;
bool wormholeActive = false;
bool darkMatterActive = false;
bool antimatterActive = false;
bool dimensionActive = false;
bool tachyonActive = false;
bool singularityActive = false;

float analogNoise() {
  return analogRead(A0) / 1023.0;
}

int scaledRange(int minValue, int maxValue) {
  float noise = analogNoise();
  return minValue + static_cast<int>((maxValue - minValue) * noise);
}

float scaledRangeFloat(float minValue, float maxValue) {
  float noise = analogNoise();
  return minValue + (maxValue - minValue) * noise;
}

String pickLabel(const char* const* labels, int count, unsigned long divisor = 1000) {
  int index = (millis() / divisor) % count;
  return String(labels[index]);
}

void handleApi(const char* path, WiFiClient& client) {
  if (strcmp(path, "/api/flux") == 0) {
    fluxActive = !fluxActive;
    digitalWrite(PIN_FLUX, fluxActive ? HIGH : LOW);
    const char* fluxStates[] = {"STABLE", "FLUCTUATING", "CRITICAL", "HARMONIZED"};
    String payload = "{";
    payload += "\"fluxState\":\"" + pickLabel(fluxStates, 4, 800) + "\",";
    payload += "\"particleCount\":" + String(scaledRange(1000, 9999)) + ",";
    payload += "\"entangle\":" + String(scaledRange(0, 100)) + ",";
    payload += "\"gauge\":" + String(scaledRange(20, 100));
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/time") == 0) {
    timeActive = !timeActive;
    digitalWrite(PIN_TIME, timeActive ? HIGH : LOW);
    const char* eras[] = {"2025 CE", "1885 CE", "2155 CE", "3024 CE", "476 CE", "10000 BCE"};
    String payload = "{";
    payload += "\"era\":\"" + pickLabel(eras, 6, 2200) + "\",";
    payload += "\"drift\":" + String(scaledRange(0, 5000)) + ",";
    payload += "\"causality\":" + String(scaledRangeFloat(0.5, 1.5), 3);
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/wormhole") == 0) {
    wormholeActive = !wormholeActive;
    digitalWrite(PIN_WORMHOLE, wormholeActive ? HIGH : LOW);
    const char* dests[] = {"ALPHA CENTAURI", "ANDROMEDA", "PARALLEL EARTH", "VOID", "PROXIMA B"};
    int stability = scaledRange(0, 100);
    String payload = "{";
    payload += "\"stability\":" + String(stability) + ",";
    payload += "\"aperture\":" + String(scaledRangeFloat(0.0, 100.0), 2) + ",";
    payload += "\"destination\":\"" + pickLabel(dests, 5, 1800) + "\",";
    payload += "\"gauge\":" + String(stability);
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/dark-matter") == 0) {
    darkMatterActive = !darkMatterActive;
    digitalWrite(PIN_DARK_MATTER, darkMatterActive ? HIGH : LOW);
    const char* flucts[] = {"MINIMAL", "MODERATE", "SEVERE", "ANOMALOUS"};
    String payload = "{";
    payload += "\"density\":" + String(scaledRangeFloat(0.0, 10.0), 3) + ",";
    payload += "\"fluctuation\":\"" + pickLabel(flucts, 4, 1500) + "\",";
    payload += "\"gridOnline\":" + String(darkMatterActive ? "true" : "false");
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/antimatter") == 0) {
    antimatterActive = !antimatterActive;
    digitalWrite(PIN_ANTIMATTER, antimatterActive ? HIGH : LOW);
    int containment = scaledRange(85, 100);
    String field = containment > 95 ? "MAXIMUM" : "DEGRADING";
    String payload = "{";
    payload += "\"containment\":" + String(containment) + ",";
    payload += "\"antimatter\":" + String(scaledRangeFloat(0.0, 1.0), 3) + ",";
    payload += "\"field\":\"" + field + "\",";
    payload += "\"gauge\":" + String(containment);
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/dimension") == 0) {
    dimensionActive = !dimensionActive;
    digitalWrite(PIN_DIMENSION, dimensionActive ? HIGH : LOW);
    const char* dims[] = {"3D", "4D", "5D", "2D", "11D", "FRACTIONAL"};
    String payload = "{";
    payload += "\"dimension\":\"" + pickLabel(dims, 6, 2000) + "\",";
    payload += "\"phase\":" + String(scaledRange(0, 360)) + ",";
    payload += "\"anchor\":\"" + String(dimensionActive ? "LOCKED" : "DRIFTING") + "\"";
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/tachyon") == 0) {
    tachyonActive = !tachyonActive;
    digitalWrite(PIN_TACHYON, tachyonActive ? HIGH : LOW);
    String payload = "{";
    payload += "\"pulseRate\":" + String(scaledRangeFloat(0.0, 1000.0), 1) + ",";
    payload += "\"ftl\":" + String(scaledRangeFloat(0.0, 10.0), 2) + ",";
    payload += "\"energy\":" + String(scaledRangeFloat(0.0, 999.0), 1);
    payload += "}";
    sendJson(client, payload);
    return;
  }
  if (strcmp(path, "/api/singularity") == 0) {
    singularityActive = !singularityActive;
    digitalWrite(PIN_SINGULARITY, singularityActive ? HIGH : LOW);
    const char* horizons[] = {"STABLE", "COLLAPSING"};
    int gauge = scaledRange(20, 100);
    String payload = "{";
    payload += "\"radius\":" + String(scaledRangeFloat(0.0, 100.0), 2) + ",";
    payload += "\"hawking\":" + String(scaledRangeFloat(0.0, 1000.0), 1) + ",";
    payload += "\"horizon\":\"" + pickLabel(horizons, 2, 900) + "\",";
    payload += "\"gauge\":" + String(gauge);
    payload += "}";
    sendJson(client, payload);
    return;
  }
  sendJson(client, "{\"error\":\"unknown command\"}", "HTTP/1.1 404 Not Found");
}

void servePage(const char* path, WiFiClient& client) {
  if (strcmp(path, "/styles.css") == 0) {
    sendStylesheet(client);
    return;
  }
  if (strcmp(path, "/app.js") == 0) {
    sendAppScript(client);
    return;
  }
  if (strncmp(path, "/api/", 5) == 0) {
    handleApi(path, client);
    return;
  }
  if (strcmp(path, "/") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", INDEX_CONTENT);
    return;
  }
  if (strcmp(path, "/systems") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", SYSTEMS_CONTENT);
    return;
  }
  if (strcmp(path, "/map") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", MAP_CONTENT);
    return;
  }
  if (strcmp(path, "/telemetry") == 0) {
    sendHtmlPage(client, "HTTP/1.1 200 OK", TELEMETRY_CONTENT);
    return;
  }
  sendHtmlPage(client, "HTTP/1.1 404 Not Found", NOT_FOUND_CONTENT);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Initializing Quantum Systems...");

  pinMode(PIN_FLUX, OUTPUT);
  pinMode(PIN_TIME, OUTPUT);
  pinMode(PIN_WORMHOLE, OUTPUT);
  pinMode(PIN_DARK_MATTER, OUTPUT);
  pinMode(PIN_ANTIMATTER, OUTPUT);
  pinMode(PIN_DIMENSION, OUTPUT);
  pinMode(PIN_TACHYON, OUTPUT);
  pinMode(PIN_SINGULARITY, OUTPUT);
  digitalWrite(PIN_FLUX, LOW);
  digitalWrite(PIN_TIME, LOW);
  digitalWrite(PIN_WORMHOLE, LOW);
  digitalWrite(PIN_DARK_MATTER, LOW);
  digitalWrite(PIN_ANTIMATTER, LOW);
  digitalWrite(PIN_DIMENSION, LOW);
  digitalWrite(PIN_TACHYON, LOW);
  digitalWrite(PIN_SINGULARITY, LOW);

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
