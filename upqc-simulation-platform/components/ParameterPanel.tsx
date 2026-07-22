'use client';

import React, { useState, useEffect } from 'react';
import { SimulationParameters } from '@/lib/simulation-types';
import { Check, RotateCcw } from 'lucide-react';

interface ParameterPanelProps {
  selectedNodeId: string | null;
  globalParameters: SimulationParameters;
  onApply: (params: SimulationParameters) => void;
  onReset: () => void;
}

export default function ParameterPanel({
  selectedNodeId,
  globalParameters,
  onApply,
  onReset,
}: ParameterPanelProps) {
  const [localParams, setLocalParams] = useState<SimulationParameters>({ ...globalParameters });

  useEffect(() => {
    setLocalParams({ ...globalParameters });
  }, [globalParameters]);

  const handleChange = (key: keyof SimulationParameters, value: number) => {
    setLocalParams((prev) => ({
      ...prev,
      [key]: value,
    }));
  };

  const handleApply = (e: React.FormEvent) => {
    e.preventDefault();
    onApply(localParams);
  };

  const renderInput = (
    label: string,
    key: keyof SimulationParameters,
    unit: string,
    step = 1,
    min = 0,
    max = 100000
  ) => {
    return (
      <div className="space-y-1">
        <label className="text-[9.5px] font-bold text-slate-400 uppercase tracking-wide flex justify-between">
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
          className="w-full bg-[#050912] border border-slate-800 rounded px-2.5 py-1 text-xs text-amber-400 font-mono focus:border-cyan-400 focus:outline-none"
        />
      </div>
    );
  };

  return (
    <div className="w-52 bg-[#060b13] border-l border-slate-850 flex flex-col h-full overflow-hidden text-slate-300 select-none font-mono text-[10px]">
      {/* Header */}
      <div className="px-3 py-2 bg-[#090f1a] text-[10px] font-bold uppercase tracking-wider text-slate-400 border-b border-slate-850 flex items-center justify-between">
        <span>Parameter Editor</span>
        <span className="text-[9px] text-amber-400 font-bold">
          BLOCK: {selectedNodeId ? selectedNodeId.toUpperCase() : 'MICROGRID'}
        </span>
      </div>

      <form onSubmit={handleApply} className="flex-1 overflow-y-auto p-3.5 space-y-4">
        {/* Section 1: Grid Source */}
        <div className="space-y-3">
          <div className="border-b border-slate-850 pb-1">
            <h3 className="text-[10px] font-bold text-cyan-400 uppercase">Grid Source</h3>
          </div>
          {renderInput('Grid Voltage (V L-L RMS)', 'gridVoltage', 'V', 1, 50, 1000)}
          {renderInput('Grid Frequency', 'gridFrequency', 'Hz', 0.5, 45, 65)}
          {renderInput('Source Resistance', 'gridRs', 'Ω', 0.01, 0.001, 10)}
          {renderInput('Source Inductance', 'gridLs', 'H', 0.0001, 0.0001, 0.5)}
        </div>

        {/* Section 2: Microgrid & Solar */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-850 pb-1">
            <h3 className="text-[10px] font-bold text-amber-400 uppercase">Microgrid & Solar</h3>
          </div>
          {renderInput('Solar Panel Count', 'solarPanelCount', 'units', 1, 1, 100)}
          {renderInput('Panel Rating', 'solarPanelWatts', 'W', 5, 50, 1000)}
          {renderInput('Solar Irradiance', 'solarIrradiance', 'W/m²', 50, 0, 1500)}
          {renderInput('Wind Speed', 'windSpeed', 'm/s', 0.5, 0, 30)}
          {renderInput('Battery State of Charge', 'batterySOC', '%', 1, 0, 100)}
          {renderInput('DC Link Target Voltage', 'dcLinkVoltage', 'V', 5, 200, 1500)}
        </div>
      </form>

      {/* Action Buttons */}
      <div className="p-3 bg-[#090f1a] border-t border-slate-850 grid grid-cols-2 gap-2">
        <button
          type="button"
          onClick={onReset}
          className="flex items-center justify-center space-x-1.5 px-3 py-2 rounded text-xs bg-slate-900 hover:bg-slate-800 border border-slate-700 text-slate-300 transition-colors"
        >
          <RotateCcw className="w-3.5 h-3.5" />
          <span>Reset</span>
        </button>

        <button
          type="button"
          onClick={handleApply}
          className="flex items-center justify-center space-x-1.5 px-3 py-2 rounded text-xs bg-emerald-600 hover:bg-emerald-500 text-white font-bold border border-emerald-400/50 shadow-md transition-colors"
        >
          <Check className="w-3.5 h-3.5" />
          <span>Apply</span>
        </button>
      </div>
    </div>
  );
}
