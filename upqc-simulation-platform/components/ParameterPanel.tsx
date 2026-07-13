'use client';

import React, { useState, useEffect } from 'react';
import { SimulationParameters } from '@/lib/simulation-types';
import { Save, RefreshCw, Info } from 'lucide-react';

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
  // Local state to hold form inputs before applying
  const [localParams, setLocalParams] = useState<SimulationParameters>({ ...globalParameters });

  // Sync local inputs when global parameters reset or change from outside
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

  const handleLocalReset = () => {
    onReset();
  };

  // Helper to render input field
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
        <label className="text-[10px] font-bold text-slate-400 uppercase tracking-wide flex justify-between">
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
          className="w-full bg-slate-900 border border-slate-800 rounded px-2 py-1 text-xs text-amber-400 font-mono focus:border-blue-500 focus:outline-none"
        />
      </div>
    );
  };

  const renderContent = () => {
    switch (selectedNodeId) {
      case 'grid-source':
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Grid parameters</h3>
              <p className="text-[10px] text-slate-500">Configures the main three-phase source generator.</p>
            </div>
            {renderInput('Grid Voltage (L-L RMS)', 'gridVoltage', 'V', 1, 50, 1000)}
            {renderInput('Grid Frequency', 'gridFrequency', 'Hz', 0.5, 45, 65)}
            {renderInput('Source Resistance', 'gridRs', 'Ω', 0.01, 0.001, 10)}
            {renderInput('Source Inductance', 'gridLs', 'H', 0.0001, 0.0001, 0.5)}
          </div>
        );
      case 'microgrid':
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Microgrid Parameters</h3>
              <p className="text-[10px] text-slate-500">Configures the local renewable energy generation system.</p>
            </div>
            {renderInput('Solar Irradiance', 'solarIrradiance', 'W/m²', 50, 0, 1500)}
            {renderInput('Wind Speed', 'windSpeed', 'm/s', 0.5, 0, 30)}
            {renderInput('Battery State of Charge', 'batterySOC', '%', 1, 0, 100)}
            {renderInput('DC Link Target Voltage', 'dcLinkVoltage', 'V', 5, 200, 1500)}
          </div>
        );
      case 'series-ctrl':
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Series APF Control</h3>
              <p className="text-[10px] text-slate-500">PI controller parameters for voltage sag injection.</p>
            </div>
            {renderInput('Proportional Gain (Kp)', 'kpSeries', 'pu', 0.1, 0, 100)}
            {renderInput('Integral Gain (Ki)', 'kiSeries', 'pu', 5, 0, 5000)}
            {renderInput('Reference Load Voltage', 'refVoltSeries', 'V', 5, 100, 600)}
          </div>
        );
      case 'shunt-ctrl':
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Shunt APF Control</h3>
              <p className="text-[10px] text-slate-500">PI controller parameters for harmonic current mitigation.</p>
            </div>
            {renderInput('Proportional Gain (Kp)', 'kpShunt', 'pu', 0.1, 0, 100)}
            {renderInput('Integral Gain (Ki)', 'kiShunt', 'pu', 5, 0, 5000)}
          </div>
        );
      case 'dc-volt-ctrl':
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Voltage Control</h3>
              <p className="text-[10px] text-slate-500">PI regulator parameters to maintain DC link capacitor charge.</p>
            </div>
            {renderInput('Proportional Gain (Kp)', 'refVoltShunt', 'pu', 0.05, 0, 50)} // mapping reference
          </div>
        );
      default:
        // Global configuration view when no node is selected
        return (
          <div className="space-y-4">
            <div className="border-b border-slate-800 pb-2">
              <h3 className="text-xs font-bold text-slate-200 uppercase font-mono">Global Model Info</h3>
              <p className="text-[10px] text-slate-500">Select an explorer item or canvas block to configure details. Below is the system-wide summary.</p>
            </div>
            <div className="bg-slate-900 border border-slate-850 p-2.5 rounded text-[11px] space-y-2 text-slate-400 font-mono">
              <div className="flex justify-between">
                <span>Model Name:</span>
                <span className="text-slate-200 font-bold">UPQC_Model_v1</span>
              </div>
              <div className="flex justify-between">
                <span>Total Nodes:</span>
                <span className="text-slate-250">14 Active Blocks</span>
              </div>
              <div className="flex justify-between">
                <span>Active Channels:</span>
                <span className="text-slate-250">7 Time-Series</span>
              </div>
              <div className="flex justify-between">
                <span>Solver:</span>
                <span className="text-amber-500 font-bold font-mono">Discrete 2e-5s</span>
              </div>
            </div>

            <div className="bg-blue-950/40 border border-blue-900/60 p-3 rounded text-[11px] text-blue-300 flex items-start space-x-2">
              <Info className="w-4 h-4 flex-shrink-0 mt-0.5" />
              <span>
                To configure specific parameters (e.g. Solar Irradiance, Grid Voltage, Kp/Ki gains), click on <strong>Grid Source</strong>, <strong>Microgrid</strong>, or <strong>Control blocks</strong>.
              </span>
            </div>
          </div>
        );
    }
  };

  return (
    <div className="w-80 bg-slate-900 border-l border-slate-800 flex flex-col h-full overflow-hidden text-slate-300 select-none">
      <div className="px-3 py-2 bg-slate-950 text-[10px] font-bold uppercase tracking-wider text-slate-400 border-b border-slate-800 flex items-center justify-between">
        <span>Parameter Editor</span>
        <span className="text-amber-500 font-mono text-[9px]">
          {selectedNodeId ? `Block: ${selectedNodeId}` : 'Global'}
        </span>
      </div>

      <form onSubmit={handleApply} className="flex-1 overflow-y-auto p-4 space-y-4">
        {renderContent()}
      </form>

      {/* Control Buttons */}
      <div className="p-3 bg-slate-950 border-t border-slate-800 grid grid-cols-2 gap-2">
        <button
          type="button"
          onClick={handleLocalReset}
          className="flex items-center justify-center space-x-1.5 px-3 py-1.5 rounded text-xs bg-slate-900 hover:bg-slate-800 border border-slate-800 text-slate-400 transition-colors"
        >
          <RefreshCw className="w-3.5 h-3.5" />
          <span>Reset</span>
        </button>
        <button
          type="button"
          onClick={handleApply}
          className="flex items-center justify-center space-x-1.5 px-3 py-1.5 rounded text-xs bg-blue-600 hover:bg-blue-500 text-white font-medium shadow-sm transition-colors"
        >
          <Save className="w-3.5 h-3.5" />
          <span>Apply</span>
        </button>
      </div>
    </div>
  );
}
