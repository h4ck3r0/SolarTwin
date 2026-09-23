'use client';

import React, { useState, useEffect } from 'react';
import { SimulationParameters } from '@/lib/simulation-types';
import { Check, RotateCcw, CloudLightning } from 'lucide-react';

interface ParameterPanelProps {
  selectedNodeId: string | null;
  selectedNodeType?: string;
  globalParameters: SimulationParameters;
  nodeParameters?: Partial<SimulationParameters>;
  onApply: (params: SimulationParameters) => void;
  onApplyNode?: (nodeId: string, params: Partial<SimulationParameters>) => void;
  onReset: () => void;
  /** Live edges from ReactFlow — used to detect which components are connected */
  connectedNodeIds?: Set<string>;
}

export default function ParameterPanel({
  selectedNodeId,
  selectedNodeType,
  globalParameters,
  nodeParameters,
  onApply,
  onApplyNode,
  onReset,
  connectedNodeIds = new Set(),
}: ParameterPanelProps) {
  const [localParams, setLocalParams] = useState<any>({ ...globalParameters, ...(nodeParameters || {}) });

  useEffect(() => {
    setLocalParams({ ...globalParameters, ...(nodeParameters || {}) });
  }, [globalParameters, selectedNodeId, nodeParameters]);

  const handleChange = (key: keyof SimulationParameters, value: any) => {
    setLocalParams((prev: any) => ({
      ...prev,
      [key]: value,
    }));
  };

  const handleApply = (e: React.FormEvent) => {
    e.preventDefault();
    const finalParams: any = { ...localParams };
    for (const k in finalParams) {
      if (typeof finalParams[k] === 'string') {
        if (finalParams[k] === '') {
            finalParams[k] = 0;
        } else {
            const parsed = Number(finalParams[k]);
            if (!isNaN(parsed)) finalParams[k] = parsed;
        }
      }
    }
    if (selectedNodeId && onApplyNode) {
        onApplyNode(selectedNodeId, finalParams);
    } else {
        onApply(finalParams as SimulationParameters);
    }
  };

  const fetchLiveWeather = async () => {
    const city = window.prompt("Enter City Name (e.g. London, Tokyo) to fetch live solar data:");
    if (!city) return;

    try {
      // 1. Geocoding
      const geoRes = await fetch(`https://geocoding-api.open-meteo.com/v1/search?name=${encodeURIComponent(city)}&count=1`);
      const geoData = await geoRes.json();
      if (!geoData.results || geoData.results.length === 0) {
        alert(`City not found: ${city}`);
        return;
      }
      
      const { latitude, longitude, name, country } = geoData.results[0];
      
      // 2. Weather fetching
      const weatherRes = await fetch(`https://api.open-meteo.com/v1/forecast?latitude=${latitude}&longitude=${longitude}&current=temperature_2m,shortwave_radiation,wind_speed_10m`);
      const weatherData = await weatherRes.json();
      
      if (weatherData.current) {
        const temp = weatherData.current.temperature_2m;
        const irrad = weatherData.current.shortwave_radiation;
        const wind = weatherData.current.wind_speed_10m;
        
        setLocalParams((prev: any) => ({
          ...prev,
          solarTemperature: temp,
          solarIrradiance: irrad,
          windSpeed: wind
        }));
        
        alert(`Successfully fetched live data for ${name}, ${country}:\nIrradiance: ${irrad} W/m²\nTemperature: ${temp} °C\nWind Speed: ${wind} m/s`);
      }
    } catch (err) {
      alert("Error fetching live weather: " + err);
    }
  };

  const renderInput = (
    label: string,
    key: keyof SimulationParameters,
    unit: string,
    step = 1,
    min = 0,
    max = 100000
  ) => {
    // FIX BUG-10: removed dead topology flag block that was computed here but never used
    return (
      <div className="space-y-1">
        <label className="text-[9.5px] font-bold text-slate-600 uppercase tracking-wide flex justify-between">
          <span>{label}</span>
          <span className="text-slate-500 font-mono font-normal">({unit})</span>
        </label>
        <input
          type="number"
          step={step}
          min={min}
          max={max}
          value={localParams[key] ?? ''}
          onChange={(e) => handleChange(key, e.target.value)}
          className="w-full bg-white border border-slate-300 rounded px-2.5 py-1 text-xs text-slate-900 font-mono focus:border-sky-500 focus:outline-none"
        />
      </div>
    );
  };

  const renderStringInput = (label: string, key: keyof SimulationParameters) => (
    <div className="space-y-1">
      <label className="text-[9.5px] font-bold text-slate-600 uppercase tracking-wide flex justify-between">
        <span>{label}</span>
      </label>
      <input
        type="text"
        value={localParams[key] ?? ''}
        onChange={(e) => handleChange(key, e.target.value as any)}
        placeholder="e.g. 0:1000, 1:500"
        className="w-full bg-white border border-slate-300 rounded px-2.5 py-1 text-xs text-slate-900 font-mono focus:border-sky-500 focus:outline-none"
      />
    </div>
  );

  const renderSelect = (label: string, key: keyof SimulationParameters, options: string[]) => (
    <div className="space-y-1">
      <label className="text-[9.5px] font-bold text-slate-600 uppercase tracking-wide flex justify-between">
        <span>{label}</span>
      </label>
      <select
        value={localParams[key] ?? options[0]}
        onChange={(e) => handleChange(key, e.target.value as any)}
        className="w-full bg-white border border-slate-300 rounded px-2.5 py-1 text-[10px] text-slate-900 font-mono focus:border-sky-500 focus:outline-none"
      >
        {options.map(opt => <option key={opt} value={opt}>{opt}</option>)}
      </select>
    </div>
  );

  // Topology-driven section visibility
  // A section is shown only when the corresponding node type is wired in the canvas
  const hasSolar   = [...connectedNodeIds].some(id => id.includes('solar') || id.includes('microgrid') || id.includes('pv'));
  const hasBattery = [...connectedNodeIds].some(id => id.includes('battery') || id.includes('bess'));
  const hasWind    = [...connectedNodeIds].some(id => id.includes('wind') || id.includes('turbine'));
  const hasGrid    = [...connectedNodeIds].some(id => id.includes('grid'));
  const hasLoad    = [...connectedNodeIds].some(id => id.includes('load'));
  const hasUPQC    = [...connectedNodeIds].some(id => id.includes('series') || id.includes('shunt') || id.includes('upqc'));
  // If nothing is connected yet (fresh canvas), show everything
  const nothingConnected = connectedNodeIds.size === 0;

  return (
    <div className="w-48 bg-white border-l border-slate-200 flex flex-col h-full overflow-hidden text-slate-700 select-none font-mono text-[10px]">
      {/* Header */}
      <div className="px-3 py-2 bg-slate-50 text-[10px] font-bold uppercase tracking-wider text-slate-500 border-b border-slate-200 flex items-center justify-between">
        <span>Global Settings</span>
      </div>

      
      <form onSubmit={(e) => { e.preventDefault(); handleApply(e as any); }} className="flex-1 overflow-y-auto p-3.5 space-y-5">

        {/* ── Simulation Engine — always visible ───────────── */}
        <div className="space-y-2">
          <h3 className="text-[9px] font-black text-indigo-700 uppercase tracking-widest border-b border-indigo-100 pb-1">Engine</h3>
          {renderInput('Duration', 'simulationDuration', 's', 0.1, 0.1, 10)}
          {renderStringInput('Irradiance Profile (t:W/m²)', 'irradianceProfile')}
          {renderStringInput('Temperature Profile (t:°C)', 'temperatureProfile')}
        </div>

        {/* ── AC Microgrid ─ show if grid node connected ────── */}
        {(nothingConnected || hasGrid) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-emerald-700 uppercase tracking-widest border-b border-emerald-100 pb-1">AC Microgrid</h3>
            {renderInput('Nominal Voltage', 'microgridVoltage', 'V', 5, 100, 1000)}
            {renderInput('Frequency', 'microgridFrequency', 'Hz', 1, 40, 60)}
            {renderInput('Grid Resistance', 'gridResistance', 'Ω', 0.01, 0, 10)}
            {renderInput('Grid Reactance', 'gridReactance', 'Ω', 0.01, 0, 10)}
          </div>
        )}

        {/* ── Load ─ show if load node connected ───────────── */}
        {(nothingConnected || hasLoad) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-rose-700 uppercase tracking-widest border-b border-rose-100 pb-1">Load</h3>
            {renderInput('Active Power', 'loadActivePower', 'kW', 1, 0, 2000)}
            {renderInput('Power Factor', 'loadPowerFactor', 'cosφ', 0.01, 0.1, 1.0)}
            {renderSelect('Harmonic Type', 'loadHarmonicType', ['Rectifier', 'VFD', 'ArcFurnace', 'Clean'])}
            {renderInput('Custom THD', 'loadTHD', '%', 1, 0, 100)}
          </div>
        )}

        {/* ── Solar PV ─ show if solar node connected ───────── */}
        {(nothingConnected || hasSolar) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-amber-700 uppercase tracking-widest border-b border-amber-100 pb-1">
              Solar PV {!nothingConnected && hasSolar && <span className="text-[8px] font-normal text-amber-500 ml-1">● connected</span>}
            </h3>
            <button type="button" onClick={fetchLiveWeather}
              className="w-full flex items-center justify-center gap-1.5 bg-amber-50 hover:bg-amber-100 text-amber-800 py-1.5 rounded border border-amber-200 text-[9px] font-bold uppercase tracking-wide transition-colors">
              <CloudLightning className="w-3 h-3" /> Sync Live Weather
            </button>
            {renderInput('Irradiance', 'solarIrradiance', 'W/m²', 50, 0, 1500)}
            {renderInput('Panel Temp', 'solarTemperature', '°C', 1, -20, 100)}
            {renderInput('Parallel Strings', 'solarStringsParallel', 'strings', 1, 1, 500)}
            {renderInput('Series Modules', 'solarModulesSeries', 'units', 1, 1, 50)}
            {renderInput('Panel Watt-Peak', 'solarPanelWatts', 'Wp', 5, 50, 1000)}
            {renderInput('Module Vmpp (STC)', 'solarVmpp', 'V', 0.5, 10, 80)}
          </div>
        )}

        {/* ── Battery ─ show only if battery node connected ──── */}
        {(nothingConnected || hasBattery) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-sky-700 uppercase tracking-widest border-b border-sky-100 pb-1">
              Battery BESS {!nothingConnected && hasBattery && <span className="text-[8px] font-normal text-sky-500 ml-1">⚡ ∥ DC Bus</span>}
            </h3>
            {!hasBattery && !nothingConnected && (
              <div className="text-[9px] text-slate-400 italic text-center py-1">
                Connect battery node to DC Link to enable
              </div>
            )}
            {renderInput('State of Charge', 'batterySOC', '%', 1, 0, 100)}
            {renderInput('Capacity', 'batteryCapacityKwh', 'kWh', 10, 0, 10000)}
          </div>
        )}

        {/* ── UPQC ─ show if UPQC/inverter node connected ───── */}
        {(nothingConnected || hasUPQC) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-purple-700 uppercase tracking-widest border-b border-purple-100 pb-1">UPQC</h3>
            {renderInput('DC Link Voltage', 'dcLinkVoltage', 'V', 10, 100, 2000)}
            {renderInput('Filter Inductance', 'filterInductance', 'mH', 0.1, 0.1, 100)}
            {renderInput('DC Capacitance', 'dcCapacitance', 'μF', 10, 10, 50000)}
            {renderInput('Kp (PI)', 'kp', 'gain', 0.1, 0, 100)}
            {renderInput('Ki (PI)', 'ki', 'gain', 1, 0, 500)}
          </div>
        )}

        {/* ── Wind ─ show only if wind turbine node connected ── */}
        {(nothingConnected || hasWind) && (
          <div className="space-y-2">
            <h3 className="text-[9px] font-black text-cyan-700 uppercase tracking-widest border-b border-cyan-100 pb-1">
              Wind Turbine {!nothingConnected && hasWind && <span className="text-[8px] font-normal text-cyan-500 ml-1">🌀 ∥ AC Bus</span>}
            </h3>
            {!hasWind && !nothingConnected && (
              <div className="text-[9px] text-slate-400 italic text-center py-1">
                Connect wind turbine to AC Bus to enable
              </div>
            )}
            {renderInput('Wind Speed', 'windSpeed', 'm/s', 0.5, 0, 40)}
            {renderInput('Cut-In Speed', 'windCutIn', 'm/s', 0.5, 0, 10)}
            {renderInput('Cut-Out Speed', 'windCutOut', 'm/s', 1, 15, 45)}
            {renderInput('Nominal Power', 'windNominalPower', 'kW', 10, 0, 10000)}
          </div>
        )}

      </form>


      {/* Action Buttons */}
      <div className="p-3 bg-slate-50 border-t border-slate-200 grid grid-cols-2 gap-2">
        <button
          type="button"
          onClick={onReset}
          className="flex items-center justify-center space-x-1.5 px-3 py-2 rounded text-xs bg-white hover:bg-slate-50 border border-slate-200 text-slate-700 transition-all duration-150 active:scale-95 shadow-sm"
        >
          <RotateCcw className="w-3.5 h-3.5" />
          <span>Reset</span>
        </button>

        <button
          type="button"
          onClick={handleApply}
          className="flex items-center justify-center space-x-1.5 px-3 py-2 rounded text-xs bg-emerald-600 hover:bg-emerald-500 text-white font-bold border border-emerald-400/50 shadow-md transition-all duration-150 active:scale-95"
        >
          <Check className="w-3.5 h-3.5" />
          <span>Apply</span>
        </button>
      </div>
    </div>
  );
}
