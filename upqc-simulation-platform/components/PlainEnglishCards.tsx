import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { Zap, Sun, Battery, Activity, ShieldAlert, Cpu } from 'lucide-react';

export default function PlainEnglishCards({ logic }: { logic: LogicState }) {
  const getCardStyle = (state: string) => {
    if (state.includes('Generating') || state.includes('Supplying') || state.includes('Active')) {
      return {
        bg: 'bg-emerald-900/20',
        border: 'border-emerald-500/50',
        shadow: 'shadow-[inset_4px_0_0_0_rgba(16,185,129,1),0_0_20px_-5px_rgba(16,185,129,0.2)]',
        text: 'text-emerald-400',
        badgeBg: 'bg-emerald-500/20 text-emerald-300 border border-emerald-500/30'
      };
    }
    if (state.includes('Charging') || state.includes('Exporting') || state.includes('Absorbing')) {
      return {
        bg: 'bg-sky-900/20',
        border: 'border-sky-500/50',
        shadow: 'shadow-[inset_4px_0_0_0_rgba(14,165,233,1),0_0_20px_-5px_rgba(14,165,233,0.2)]',
        text: 'text-sky-400',
        badgeBg: 'bg-sky-500/20 text-sky-300 border border-sky-500/30'
      };
    }
    if (state.includes('Discharging') || state.includes('Sag') || state.includes('Swell')) {
      return {
        bg: 'bg-amber-900/20',
        border: 'border-amber-500/50',
        shadow: 'shadow-[inset_4px_0_0_0_rgba(245,158,11,1),0_0_20px_-5px_rgba(245,158,11,0.2)]',
        text: 'text-amber-400',
        badgeBg: 'bg-amber-500/20 text-amber-300 border border-amber-500/30'
      };
    }
    return {
      bg: 'bg-slate-800/50',
      border: 'border-slate-700',
      shadow: 'shadow-[inset_4px_0_0_0_rgba(100,116,139,1)]',
      text: 'text-slate-400',
      badgeBg: 'bg-slate-700/50 text-slate-300 border border-slate-600'
    };
  };

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 gap-6">
      
      {/* Solar Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-x-1 backdrop-blur-sm
        ${getCardStyle(logic.solar.state).bg} ${getCardStyle(logic.solar.state).border} ${getCardStyle(logic.solar.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg bg-slate-900/50 border border-slate-700 ${getCardStyle(logic.solar.state).text}`}>
            <Sun className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">Solar Generation</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.solar.state).badgeBg}`}>
            {logic.solar.state}
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">{logic.solar.description}</p>
      </div>

      {/* Grid Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-x-1 backdrop-blur-sm
        ${getCardStyle(logic.grid.state).bg} ${getCardStyle(logic.grid.state).border} ${getCardStyle(logic.grid.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg bg-slate-900/50 border border-slate-700 ${getCardStyle(logic.grid.state).text}`}>
            <Zap className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">Main Grid</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.grid.state).badgeBg}`}>
            {logic.grid.state}
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">{logic.grid.description}</p>
      </div>

      {/* Battery Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-x-1 backdrop-blur-sm
        ${getCardStyle(logic.battery.state).bg} ${getCardStyle(logic.battery.state).border} ${getCardStyle(logic.battery.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg bg-slate-900/50 border border-slate-700 ${getCardStyle(logic.battery.state).text}`}>
            <Battery className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">Battery Storage</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.battery.state).badgeBg}`}>
            {logic.battery.state}
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">{logic.battery.description}</p>
      </div>

      {/* UPQC Series Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-x-1 backdrop-blur-sm
        ${getCardStyle(logic.upqcSeries.state).bg} ${getCardStyle(logic.upqcSeries.state).border} ${getCardStyle(logic.upqcSeries.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg bg-slate-900/50 border border-slate-700 ${getCardStyle(logic.upqcSeries.state).text}`}>
            <ShieldAlert className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">UPQC Series Filter</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.upqcSeries.state).badgeBg}`}>
            {logic.upqcSeries.state}
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">{logic.upqcSeries.description}</p>
      </div>

      {/* UPQC Shunt Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-x-1 backdrop-blur-sm
        ${getCardStyle(logic.upqcShunt.state).bg} ${getCardStyle(logic.upqcShunt.state).border} ${getCardStyle(logic.upqcShunt.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg bg-slate-900/50 border border-slate-700 ${getCardStyle(logic.upqcShunt.state).text}`}>
            <Activity className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">UPQC Shunt Filter</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.upqcShunt.state).badgeBg}`}>
            {logic.upqcShunt.state}
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">{logic.upqcShunt.description}</p>
      </div>
      
      {/* System Intel */}
      <div className="rounded-xl p-6 border border-indigo-500/50 bg-indigo-900/20 shadow-[inset_4px_0_0_0_rgba(99,102,241,1),0_0_20px_-5px_rgba(99,102,241,0.2)] transition-all duration-300 hover:translate-x-1 backdrop-blur-sm">
        <div className="flex items-center gap-3 mb-4">
          <div className="p-2 rounded-lg bg-slate-900/50 border border-slate-700 text-indigo-400">
            <Cpu className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-200 text-lg tracking-wide">AI Control Agent</h3>
        </div>
        <div className="mb-4">
          <span className="text-xs font-bold uppercase px-3 py-1.5 rounded-full bg-indigo-500/20 text-indigo-300 border border-indigo-500/30">
            Active
          </span>
        </div>
        <p className="text-sm text-slate-400 leading-relaxed min-h-[60px]">
          The Antigravity Engine is actively coordinating power flows and monitoring the dc-link voltage to ensure seamless transitions between grid, solar, and battery power.
        </p>
      </div>

    </div>
  );
}
