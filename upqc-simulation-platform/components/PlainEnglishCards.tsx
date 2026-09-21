import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { Zap, Sun, Battery, Activity, ShieldAlert, Cpu } from 'lucide-react';

export default function PlainEnglishCards({ logic }: { logic: LogicState }) {
  const getCardStyle = (state: string) => {
    if (state.includes('Generating') || state.includes('Supplying') || state.includes('Active')) {
      return {
        bg: 'bg-emerald-50',
        border: 'border-emerald-200',
        shadow: 'shadow-sm shadow-emerald-100',
        text: 'text-emerald-600',
        badgeBg: 'bg-emerald-100 text-emerald-700 border border-emerald-200',
        iconBg: 'bg-white border-emerald-100'
      };
    }
    if (state.includes('Charging') || state.includes('Exporting') || state.includes('Absorbing')) {
      return {
        bg: 'bg-sky-50',
        border: 'border-sky-200',
        shadow: 'shadow-sm shadow-sky-100',
        text: 'text-sky-600',
        badgeBg: 'bg-sky-100 text-sky-700 border border-sky-200',
        iconBg: 'bg-white border-sky-100'
      };
    }
    if (state.includes('Discharging') || state.includes('Sag') || state.includes('Swell')) {
      return {
        bg: 'bg-amber-50',
        border: 'border-amber-200',
        shadow: 'shadow-sm shadow-amber-100',
        text: 'text-amber-600',
        badgeBg: 'bg-amber-100 text-amber-700 border border-amber-200',
        iconBg: 'bg-white border-amber-100'
      };
    }
    return {
      bg: 'bg-white',
      border: 'border-slate-200',
      shadow: 'shadow-sm',
      text: 'text-slate-500',
      badgeBg: 'bg-slate-100 text-slate-600 border border-slate-200',
      iconBg: 'bg-slate-50 border-slate-100'
    };
  };

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 xl:grid-cols-3 gap-6">
      
      {/* Solar Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md
        ${getCardStyle(logic.solar.state).bg} ${getCardStyle(logic.solar.state).border} ${getCardStyle(logic.solar.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg border ${getCardStyle(logic.solar.state).iconBg} ${getCardStyle(logic.solar.state).text}`}>
            <Sun className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">Solar Generation</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.solar.state).badgeBg}`}>
            {logic.solar.state}
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">{logic.solar.description}</p>
      </div>

      {/* Grid Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md
        ${getCardStyle(logic.grid.state).bg} ${getCardStyle(logic.grid.state).border} ${getCardStyle(logic.grid.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg border ${getCardStyle(logic.grid.state).iconBg} ${getCardStyle(logic.grid.state).text}`}>
            <Zap className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">Main Grid</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.grid.state).badgeBg}`}>
            {logic.grid.state}
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">{logic.grid.description}</p>
      </div>

      {/* Battery Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md
        ${getCardStyle(logic.battery.state).bg} ${getCardStyle(logic.battery.state).border} ${getCardStyle(logic.battery.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg border ${getCardStyle(logic.battery.state).iconBg} ${getCardStyle(logic.battery.state).text}`}>
            <Battery className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">Battery Storage</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.battery.state).badgeBg}`}>
            {logic.battery.state}
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">{logic.battery.description}</p>
      </div>

      {/* UPQC Series Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md
        ${getCardStyle(logic.upqcSeries.state).bg} ${getCardStyle(logic.upqcSeries.state).border} ${getCardStyle(logic.upqcSeries.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg border ${getCardStyle(logic.upqcSeries.state).iconBg} ${getCardStyle(logic.upqcSeries.state).text}`}>
            <ShieldAlert className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">UPQC Series Filter</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.upqcSeries.state).badgeBg}`}>
            {logic.upqcSeries.state}
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">{logic.upqcSeries.description}</p>
      </div>

      {/* UPQC Shunt Logic */}
      <div className={`rounded-xl p-6 border transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md
        ${getCardStyle(logic.upqcShunt.state).bg} ${getCardStyle(logic.upqcShunt.state).border} ${getCardStyle(logic.upqcShunt.state).shadow}`}>
        <div className="flex items-center gap-3 mb-4">
          <div className={`p-2 rounded-lg border ${getCardStyle(logic.upqcShunt.state).iconBg} ${getCardStyle(logic.upqcShunt.state).text}`}>
            <Activity className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">UPQC Shunt Filter</h3>
        </div>
        <div className="mb-4">
          <span className={`text-xs font-bold uppercase px-3 py-1.5 rounded-full ${getCardStyle(logic.upqcShunt.state).badgeBg}`}>
            {logic.upqcShunt.state}
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">{logic.upqcShunt.description}</p>
      </div>
      
      {/* System Intel */}
      <div className="rounded-xl p-6 border border-indigo-200 bg-indigo-50 shadow-sm shadow-indigo-100 transition-all duration-300 hover:translate-y-[-2px] hover:shadow-md">
        <div className="flex items-center gap-3 mb-4">
          <div className="p-2 rounded-lg bg-white border border-indigo-100 text-indigo-600">
            <Cpu className="w-5 h-5" />
          </div>
          <h3 className="font-bold text-slate-800 text-lg tracking-wide">AI Control Agent</h3>
        </div>
        <div className="mb-4">
          <span className="text-xs font-bold uppercase px-3 py-1.5 rounded-full bg-indigo-100 text-indigo-700 border border-indigo-200">
            Active
          </span>
        </div>
        <p className="text-sm text-slate-600 leading-relaxed min-h-[60px]">
          The Antigravity Engine is actively coordinating power flows and monitoring the dc-link voltage to ensure seamless transitions between grid, solar, and battery power.
        </p>
      </div>

    </div>
  );
}
