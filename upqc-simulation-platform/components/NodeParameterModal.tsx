import React, { useState, useEffect } from 'react';
import { X, Check, Zap, Battery, Factory, Activity } from 'lucide-react';
import { SimulationParameters } from '@/lib/simulation-types';
import Image from 'next/image';

interface NodeParameterModalProps {
  nodeId: string;
  nodeType: string;
  nodeLabel: string;
  globalParameters: SimulationParameters;
  nodeParameters?: Partial<SimulationParameters>;
  onClose: () => void;
  onApply: (nodeId: string, params: Partial<SimulationParameters>) => void;
}

export default function NodeParameterModal({
  nodeId,
  nodeType,
  nodeLabel,
  globalParameters,
  nodeParameters,
  onClose,
  onApply,
}: NodeParameterModalProps) {
  const [localParams, setLocalParams] = useState<any>({ ...globalParameters, ...(nodeParameters || {}) });

  useEffect(() => {
    setLocalParams({ ...globalParameters, ...(nodeParameters || {}) });
  }, [globalParameters, nodeParameters, nodeId]);

  const handleChange = (key: keyof SimulationParameters, value: number) => {
    setLocalParams((prev: any) => ({
      ...prev,
      [key]: value,
    }));
  };

  const handleApply = (e: React.FormEvent) => {
    e.preventDefault();
    onApply(nodeId, localParams);
    onClose();
  };

  const renderInput = (label: string, key: keyof SimulationParameters, unit: string, step = 1, min = 0, max = 100000) => (
    <div className="flex flex-col space-y-1">
      <label className="text-xs font-bold text-slate-600 uppercase tracking-wide flex justify-between">
        <span>{label}</span>
        <span className="text-slate-400 font-normal">({unit})</span>
      </label>
      <input
        type="number"
        step={step}
        min={min}
        max={max}
        value={localParams[key] ?? ''}
        onChange={(e) => handleChange(key, parseFloat(e.target.value) || 0)}
        className="w-full bg-slate-50 border border-slate-300 rounded px-2.5 py-1.5 text-sm text-slate-900 font-mono focus:border-sky-500 focus:outline-none"
      />
    </div>
  );


  const nodeName = (nodeLabel || '').toLowerCase() + " " + nodeId.toLowerCase();
  const isSolar = nodeName.includes('solar') || nodeName.includes('pv');
  const isGrid = nodeName.includes('grid');
  const isLoad = nodeName.includes('load');
  const isUPQC = nodeName.includes('upqc') || nodeName.includes('filter');
  const isBattery = nodeName.includes('battery');
  
  let nodeTitle = "Component";
  if (isSolar) nodeTitle = "Solar PV Array";
  else if (isGrid) nodeTitle = "Utility Grid";
  else if (isLoad) nodeTitle = "Critical Load";
  else if (isUPQC) nodeTitle = "UPQC Filter";
  else if (isBattery) nodeTitle = "Battery Storage";


  return (
    <div className="fixed inset-0 bg-slate-900/40 backdrop-blur-sm z-[100] flex items-center justify-center p-4">
      <div className="bg-white rounded-lg shadow-2xl overflow-hidden w-full max-w-2xl flex flex-col sm:flex-row border border-slate-200">
        
        {/* Left Side: Component Image & Identity */}
        <div className="bg-slate-50 sm:w-2/5 border-b sm:border-b-0 sm:border-r border-slate-200 p-6 flex flex-col items-center justify-center space-y-4">
                    <div className="w-40 h-40 bg-white rounded-lg border border-slate-200 shadow-sm flex items-center justify-center p-2 relative overflow-hidden">
            {isSolar && <Image src="/solar.jpg" alt="Solar Array" layout="fill" objectFit="contain" />}
            {isGrid && <Zap className="w-20 h-20 text-sky-500" />}
            {isLoad && <Factory className="w-20 h-20 text-rose-500" />}
            {isUPQC && <Activity className="w-20 h-20 text-purple-500" />}
            {isBattery && <Battery className="w-20 h-20 text-emerald-500" />}
            {!isSolar && !isGrid && !isLoad && !isUPQC && !isBattery && <div className="text-slate-300 text-6xl">?</div>}
          </div>
          <div className="text-center">
            <h2 className="text-lg font-bold text-slate-800 uppercase tracking-wider">{nodeTitle}</h2>
            <p className="text-xs text-slate-500 font-mono mt-1">ID: {nodeId}</p>
          </div>
        </div>

        {/* Right Side: Form */}
        <form onSubmit={handleApply} className="sm:w-3/5 flex flex-col bg-white">
          <div className="flex items-center justify-between px-6 py-4 border-b border-slate-100">
            <h3 className="text-sm font-bold text-slate-700 uppercase tracking-wider">Edit Parameters</h3>
            <button type="button" onClick={onClose} className="text-slate-400 hover:text-slate-600 transition-colors">
              <X className="w-5 h-5" />
            </button>
          </div>
          
                    <div className="flex-1 overflow-y-auto p-6 space-y-4 max-h-[60vh]">
            {isSolar && (
              <>
                {renderInput('Parallel Strings', 'solarStringsParallel', 'units', 1, 1, 500)}
                {renderInput('Series Modules', 'solarModulesSeries', 'units', 1, 1, 50)}
                {renderInput('Panel Rating', 'solarPanelWatts', 'W', 5, 50, 1000)}
                {renderInput('Irradiance', 'solarIrradiance', 'W/m²', 50, 0, 1500)}
              </>
            )}
            
            {isLoad && (
              <>
                {renderInput('Active Power', 'loadActivePower', 'kW', 5, 0, 1000)}
                {renderInput('Power Factor', 'loadPowerFactor', 'cosφ', 0.01, 0.1, 1.0)}
                {renderInput('THD', 'loadTHD', '%', 1, 0, 100)}
              </>
            )}
            
            {isGrid && (
              <>
                <label className="flex items-center space-x-2 cursor-pointer bg-slate-50 p-3 border border-slate-200 rounded mb-4">
                  <input
                    type="checkbox"
                    checked={localParams.isGridConnected !== false}
                    onChange={(e) => handleChange('isGridConnected', e.target.checked as any)}
                    className="w-4 h-4 text-sky-600 rounded border-slate-300 focus:ring-sky-500"
                  />
                  <span className="text-sm font-bold text-slate-700">Grid Tie-Line Connected</span>
                </label>
                {renderInput('Resistance (R)', 'gridResistance', 'Ω', 0.01, 0, 10)}
                {renderInput('Reactance (X)', 'gridReactance', 'Ω', 0.01, 0, 10)}
              </>
            )}
            
            {isUPQC && (
              <>
                {renderInput('Filter Inductance', 'filterInductance', 'mH', 0.1, 0.1, 100)}
                {renderInput('DC Link Capacitance', 'dcCapacitance', 'μF', 10, 100, 10000)}
                {renderInput('Proportional Gain (Kp)', 'kp', 'gain', 0.1, 0, 50)}
                {renderInput('Integral Gain (Ki)', 'ki', 'gain', 0.1, 0, 50)}
              </>
            )}
            
            {isBattery && (
              <>
                {renderInput('State of Charge (SOC)', 'batterySOC', '%', 1, 0, 100)}
                {renderInput('Target DC Voltage', 'dcLinkVoltage', 'V', 5, 200, 1500)}
              </>
            )}
          </div>

          <div className="px-6 py-4 bg-slate-50 border-t border-slate-100 flex justify-end space-x-3">
            <button
              type="button"
              onClick={onClose}
              className="px-4 py-2 rounded text-sm font-medium text-slate-600 bg-white border border-slate-300 hover:bg-slate-50 transition-colors"
            >
              Cancel
            </button>
            <button
              type="submit"
              className="flex items-center space-x-2 px-6 py-2 rounded text-sm font-bold text-white bg-sky-600 hover:bg-sky-500 transition-all active:scale-95 shadow-md"
            >
              <Check className="w-4 h-4" />
              <span>Apply to Node</span>
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
