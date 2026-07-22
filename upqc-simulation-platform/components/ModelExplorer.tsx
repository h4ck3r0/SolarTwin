'use client';

import React, { useState } from 'react';
import { ChevronDown, ChevronRight, Zap, Sun, Cpu, Layout, Radio } from 'lucide-react';

interface ExplorerItem {
  id: string;
  name: string;
  nodeId?: string;
  details?: string;
  children?: ExplorerItem[];
}

interface ModelExplorerProps {
  selectedNodeId: string | null;
  onSelectItem: (nodeId: string) => void;
}

export default function ModelExplorer({ selectedNodeId, onSelectItem }: ModelExplorerProps) {
  const [expanded, setExpanded] = useState<Record<string, boolean>>({
    grid: true,
    series: true,
    microgrid: true,
    shunt: true,
  });

  const toggleExpand = (key: string, e: React.MouseEvent) => {
    e.stopPropagation();
    setExpanded((prev) => ({ ...prev, [key]: !prev[key] }));
  };

  const modelTree: ExplorerItem = {
    id: 'root',
    name: 'UPQC Model',
    children: [
      {
        id: 'grid',
        name: '3Φ Grid Source',
        nodeId: 'grid-source',
        children: [
          { id: 'vsrc', name: 'Vsource 415V LL' },
          { id: 'freq', name: 'Frequency 50Hz' },
          { id: 'rsls', name: 'Rs / Ls Model' },
        ],
      },
      {
        id: 'series',
        name: 'Series APF',
        children: [
          { id: 'series-ctrl', name: 'PI Voltage Ctrl', nodeId: 'series-ctrl' },
          { id: 'series-inv', name: 'IGBT H-Bridge', nodeId: 'series-inv' },
          { id: 'series-xfm', name: 'Series XFM 1:1', nodeId: 'series-trans' },
        ],
      },
      {
        id: 'microgrid',
        name: 'Microgrid Sources',
        nodeId: 'microgrid',
        children: [
          { id: 'solar', name: 'Solar Energy', nodeId: 'microgrid' },
          { id: 'wind', name: 'Wind Energy', nodeId: 'microgrid' },
          { id: 'battery', name: 'Battery', nodeId: 'microgrid' },
        ],
      },
      { id: 'voltage-ctrl', name: 'Voltage Control', nodeId: 'dc-volt-ctrl' },
      {
        id: 'shunt',
        name: 'Shunt APF',
        children: [
          { id: 'shunt-ctrl', name: 'Shunt APF Control', nodeId: 'shunt-ctrl' },
          { id: 'shunt-inv', name: 'IGBT Shunt Inverter', nodeId: 'shunt-inv' },
        ],
      },
      { id: 'diode', name: 'Diode Rectifier', nodeId: 'diode-rectifier' },
    ],
  };

  const renderTree = (item: ExplorerItem, depth = 0) => {
    const hasChildren = item.children && item.children.length > 0;
    const isExpanded = expanded[item.id];
    const isSelected = item.nodeId ? selectedNodeId === item.nodeId : false;

    const getIcon = (id: string, isLeaf: boolean) => {
      if (id === 'grid' || id.includes('vsrc')) return <Radio className="w-3.5 h-3.5 text-cyan-400" />;
      if (id.includes('series')) return <Zap className="w-3.5 h-3.5 text-amber-400" />;
      if (id.includes('microgrid') || id.includes('solar')) return <Sun className="w-3.5 h-3.5 text-amber-400" />;
      if (id.includes('ctrl')) return <Cpu className="w-3.5 h-3.5 text-cyan-400" />;
      return <Layout className="w-3.5 h-3.5 text-slate-500" />;
    };

    return (
      <div key={item.id} className="select-none">
        <div
          onClick={() => item.nodeId && onSelectItem(item.nodeId)}
          className={`flex items-center py-1.5 px-2 hover:bg-[#101726] cursor-pointer text-xs font-mono transition-colors ${
            isSelected ? 'bg-cyan-950/80 border-r-2 border-cyan-400 text-cyan-200 font-bold' : 'text-slate-300'
          }`}
          style={{ paddingLeft: `${depth * 14 + 8}px` }}
        >
          {hasChildren ? (
            <button
              onClick={(e) => toggleExpand(item.id, e)}
              className="mr-1 p-0.5 rounded text-slate-400 hover:text-slate-200"
            >
              {isExpanded ? <ChevronDown className="w-3 h-3 text-cyan-400" /> : <ChevronRight className="w-3 h-3 text-slate-500" />}
            </button>
          ) : (
            <span className="w-4 mr-1"></span>
          )}

          <span className="mr-2 flex-shrink-0">{getIcon(item.id, !hasChildren)}</span>
          <span className="truncate">{item.name}</span>
        </div>

        {hasChildren && isExpanded && (
          <div className="border-l border-slate-850 ml-3.5">
            {item.children!.map((child) => renderTree(child, depth + 1))}
          </div>
        )}
      </div>
    );
  };

  return (
    <div className="w-44 bg-[#060b13] border-r border-slate-850 flex flex-col h-full overflow-hidden text-slate-300 select-none font-mono text-[10px]">
      {/* Header */}
      <div className="px-3 py-2 bg-[#090f1a] text-[10px] font-bold uppercase tracking-wider text-slate-400 border-b border-slate-850 flex items-center justify-between">
        <span>Model Explorer</span>
        <span className="text-[9px] text-cyan-400 font-mono">1.0_UPQC</span>
      </div>

      {/* Tree list */}
      <div className="flex-1 overflow-y-auto py-2">
        {renderTree(modelTree)}
      </div>

      {/* Diagnostics */}
      <div className="p-3 bg-[#090f1a] border-t border-slate-850 text-[10px] space-y-1.5">
        <div className="text-slate-400 font-bold uppercase tracking-wide border-b border-slate-800 pb-1 text-[9px]">
          Quick Diagnostics
        </div>
        <div className="flex justify-between">
          <span className="text-slate-500">Block ID:</span>
          <span className="text-cyan-400 font-bold truncate max-w-[120px]">
            {selectedNodeId || 'grid-source'}
          </span>
        </div>
      </div>
    </div>
  );
}
