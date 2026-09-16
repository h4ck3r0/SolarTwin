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
}

export default function ParameterPanel({
  selectedNodeId,
  selectedNodeType,
  globalParameters,
  nodeParameters,
  onApply,
  onApplyNode,
  onReset,
}: ParameterPanelProps) {
  const [localParams, setLocalParams] = useState<any>({ ...globalParameters, ...(nodeParameters || {}) });

  useEffect(() => {
    setLocalParams({ ...globalParameters, ...(nodeParameters || {}) });
  }, [globalParameters, selectedNodeId, nodeParameters]);

  const handleChange = (key: keyof SimulationParameters, value: number) => {
    setLocalParams((prev: any) => ({
      ...prev,
      [key]: value,
    }));
  };

  const handleApply = (e: React.FormEvent) => {
    e.preventDefault();
    if (selectedNodeId && onApplyNode) {
        onApplyNode(selectedNodeId, localParams);
    } else {
        onApply(localParams as SimulationParameters);
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
  
  const nodeName = (selectedNodeId || '').toLowerCase() + " " + (selectedNodeType || '').toLowerCase();
  const isSolar = nodeName.includes('solar') || nodeName.includes('pv');
  const isGrid = nodeName.includes('grid');
  const isLoad = nodeName.includes('load');
  const isUPQC = nodeName.includes('upqc') || nodeName.includes('filter');
  const isBattery = nodeName.includes('battery');
  
  const showEngine = !selectedNodeId;
  const showGrid = !selectedNodeId || isGrid;
  const showSolar = !selectedNodeId || isSolar || isBattery;
  const showLoad = !selectedNodeId || isLoad;
  const showUPQC = !selectedNodeId || isUPQC;

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
          onChange={(e) => handleChange(key, parseFloat(e.target.value) || 0)}
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

  const nodeName = (selectedNodeId || '').toLowerCase() + " " + (selectedNodeType || '').toLowerCase();
  const isSolar = nodeName.includes('solar') || nodeName.includes('pv');
  const isGrid = nodeName.includes('grid');
  const isLoad = nodeName.includes('load');
  const isUPQC = nodeName.includes('upqc') || nodeName.includes('filter');
  const isBattery = nodeName.includes('battery');
  
  const showEngine = !selectedNodeId;
  const showGrid = !selectedNodeId || isGrid;
  const showSolar = !selectedNodeId || isSolar || isBattery;
  const showLoad = !selectedNodeId || isLoad;
  const showUPQC = !selectedNodeId || isUPQC;

  return (
    <div className="w-48 bg-white border-l border-slate-200 flex flex-col h-full overflow-hidden text-slate-700 select-none font-mono text-[10px]">
      {/* Header */}
      <div className="px-3 py-2 bg-slate-50 text-[10px] font-bold uppercase tracking-wider text-slate-500 border-b border-slate-200 flex items-center justify-between">
        <span>Global Settings</span>
      </div>

      <form onSubmit={handleApply} className="flex-1 overflow-y-auto p-3.5 space-y-4">
        {/* Section 0: Simulation Engine */}
        <div className="space-y-3 pt-2 pb-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-indigo-700 uppercase">Engine Settings</h3>
          </div>
          {renderInput('Duration', 'simulationDuration', 's', 0.1, 0.1, 10)}
          {renderStringInput('Irradiance Profile (s:W/m2)', 'irradianceProfile')}
        </div>
        
        {/* Section 1: AC Microgrid Bus */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-emerald-700 uppercase">AC Microgrid Base</h3>
          </div>
          {renderInput('Nominal Voltage', 'microgridVoltage', 'V', 5, 100, 1000)}
          {renderInput('Frequency', 'microgridFrequency', 'Hz', 1, 40, 60)}
        </div>
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
