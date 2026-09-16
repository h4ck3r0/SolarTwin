'use client';

import React, { useState, useEffect } from 'react';
import { SimulationParameters } from '@/lib/simulation-types';
import { Check, RotateCcw } from 'lucide-react';

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
        <span>Parameter Editor</span>
        <span className="text-[9px] text-amber-600 font-bold">
          BLOCK: {selectedNodeId ? selectedNodeId.toUpperCase() : 'MICROGRID'}
        </span>
      </div>

      <form onSubmit={handleApply} className="flex-1 overflow-y-auto p-3.5 space-y-4">
        {showEngine && (
          <>
        {/* Section 0: Simulation Engine */}
        <div className="space-y-3 pt-2 pb-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-indigo-700 uppercase">Engine Settings</h3>
          </div>
          {renderInput('Duration', 'simulationDuration', 's', 0.1, 0.1, 10)}
          {renderStringInput('Irradiance Profile (s:W/m2)', 'irradianceProfile')}
        </div>
        
          </>
        )}
        {showEngine && (
          <>
        {/* Section 1: AC Microgrid Bus */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-emerald-700 uppercase">AC Microgrid Bus</h3>
          </div>
          {renderInput('Line Voltage', 'microgridVoltage', 'V', 5, 100, 1000)}
          {renderInput('Frequency', 'microgridFrequency', 'Hz', 1, 40, 60)}
        </div>

        
          </>
        )}
        {showGrid && (
          <>
        {/* Section 1.5: Grid Impedance */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1 flex items-center justify-between">
            <h3 className="text-[10px] font-bold text-sky-700 uppercase">Grid Topology</h3>
          </div>
          
          <label className="flex items-center space-x-2 cursor-pointer bg-slate-50 p-2 border border-slate-200 rounded">
            <input
              type="checkbox"
              checked={localParams.isGridConnected !== false}
              onChange={(e) => handleChange('isGridConnected', e.target.checked as any)}
              className="w-3.5 h-3.5 text-sky-600 rounded border-slate-300 focus:ring-sky-500"
            />
            <span className="text-[10px] font-bold text-slate-700">Grid Connected (Tie-Line)</span>
          </label>

          <div className="border-b border-slate-200 pb-1 mt-3">
            <h3 className="text-[10px] font-bold text-sky-700 uppercase">Grid Impedance</h3>
          </div>
          {renderInput('R', 'gridResistance', 'Ω', 0.01, 0, 10)}
          {renderInput('X', 'gridReactance', 'Ω', 0.01, 0, 10)}
        </div>

          </>
        )}
        {showSolar && (
          <>
        {/* Section 2: Microgrid & Solar */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-amber-400 uppercase">Microgrid & Solar</h3>
          </div>
          {renderInput('Strings', 'solarStringsParallel', 'units', 1, 1, 500)}
          {renderInput('Modules', 'solarModulesSeries', 'units', 1, 1, 50)}
          {renderInput('Panel Power', 'solarPanelWatts', 'W', 5, 50, 1000)}
          {renderInput('Irradiance', 'solarIrradiance', 'W/m²', 50, 0, 1500)}
          
          {renderInput('Battery SOC', 'batterySOC', '%', 1, 0, 100)}
          {renderInput('Target VDC', 'dcLinkVoltage', 'V', 5, 200, 1500)}
        </div>
      
          </>
        )}
        {showLoad && (
          <>
        {/* Section 2.5: Critical Load */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-rose-600 uppercase">Critical Load</h3>
          </div>
          {renderInput('Power', 'loadActivePower', 'kW', 5, 0, 1000)}
          {renderInput('P.F.', 'loadPowerFactor', 'cosφ', 0.01, 0.1, 1.0)}
          {renderInput('THD', 'loadTHD', '%', 1, 0, 100)}
        </div>

      
          </>
        )}
        {showUPQC && (
          <>
        {/* Section 3: Advanced UPQC Physics */}
        <div className="space-y-3 pt-2">
          <div className="border-b border-slate-200 pb-1">
            <h3 className="text-[10px] font-bold text-purple-600 uppercase">Advanced UPQC Physics</h3>
          </div>
          {renderInput('Filter L', 'filterInductance', 'mH', 0.1, 0.1, 100)}
          {renderInput('DC Cap.', 'dcCapacitance', 'μF', 10, 100, 10000)}
          {renderInput('Kp', 'kp', 'gain', 0.1, 0, 50)}
          {renderInput('Ki', 'ki', 'gain', 0.1, 0, 50)}
        </div>

          </>
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
