'use client';

import React, { useState } from 'react';
import { ChevronDown, ChevronRight, Cpu, Zap, Radio, Layout, LineChart } from 'lucide-react';

interface ExplorerItem {
  id: string;
  name: string;
  nodeId?: string; // Maps to React Flow node ID
  children?: ExplorerItem[];
}

interface ModelExplorerProps {
  selectedNodeId: string | null;
  onSelectItem: (nodeId: string) => void;
}

export default function ModelExplorer({ selectedNodeId, onSelectItem }: ModelExplorerProps) {
  const [expanded, setExpanded] = useState<Record<string, boolean>>({
    root: true,
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
      { id: 'grid', name: 'Grid', nodeId: 'grid-source' },
      { id: 'transformer', name: 'Three Phase Transformer', nodeId: 'series-trans' },
      {
        id: 'series',
        name: 'Series APF',
        children: [
          { id: 'series-ctrl', name: 'Series APF Control', nodeId: 'series-ctrl' },
          { id: 'series-inv', name: 'IGBT Series Inverter', nodeId: 'series-inv' },
        ],
      },
      {
        id: 'microgrid',
        name: 'Microgrid',
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

    // Node icon selection
    const getIcon = (id: string, isLeaf: boolean) => {
      if (!isLeaf) return <Layout className="w-3.5 h-3.5 text-blue-400" />;
      if (id.includes('ctrl') || id.includes('voltage-ctrl')) return <Cpu className="w-3.5 h-3.5 text-sky-400" />;
      if (id.includes('inv')) return <Zap className="w-3.5 h-3.5 text-amber-400" />;
      if (id === 'grid') return <Radio className="w-3.5 h-3.5 text-green-400" />;
      return <Zap className="w-3.5 h-3.5 text-slate-400" />;
    };

    return (
      <div key={item.id} className="select-none">
        <div
          onClick={() => item.nodeId && onSelectItem(item.nodeId)}
          className={`flex items-center py-1.5 px-2 hover:bg-slate-800 cursor-pointer text-xs font-mono transition-colors ${
            isSelected ? 'bg-blue-950/80 border-r-2 border-blue-500 text-blue-200' : 'text-slate-300'
          }`}
          style={{ paddingLeft: `${depth * 12 + 8}px` }}
        >
          {hasChildren ? (
            <button
              onClick={(e) => toggleExpand(item.id, e)}
              className="mr-1 p-0.5 rounded hover:bg-slate-700 text-slate-400"
            >
              {isExpanded ? <ChevronDown className="w-3 h-3" /> : <ChevronRight className="w-3 h-3" />}
            </button>
          ) : (
            <span className="w-4 mr-1"></span>
          )}

          <span className="mr-2 flex-shrink-0">{getIcon(item.id, !hasChildren)}</span>
          <span className="truncate">{item.name}</span>
        </div>

        {hasChildren && isExpanded && (
          <div className="border-l border-slate-800 ml-3.5">
            {item.children!.map((child) => renderTree(child, depth + 1))}
          </div>
        )}
      </div>
    );
  };

  return (
    <div className="w-64 bg-slate-900 border-r border-slate-800 flex flex-col h-full overflow-hidden text-slate-300 select-none">
      <div className="px-3 py-2 bg-slate-950 text-[10px] font-bold uppercase tracking-wider text-slate-400 border-b border-slate-800 flex items-center justify-between">
        <span>Model Explorer</span>
        <span className="text-[9px] text-blue-500 font-mono">1.0_UPQC</span>
      </div>

      <div className="flex-1 overflow-y-auto py-2">
        {renderTree(modelTree)}
      </div>

      {/* Quick Diagnostics Block */}
      <div className="p-3 bg-slate-950 border-t border-slate-800 text-[10px] font-mono space-y-1.5">
        <div className="text-slate-400 font-bold uppercase tracking-wide border-b border-slate-800 pb-1">
          Diagnostics Bus
        </div>
        <div className="flex justify-between">
          <span className="text-slate-500">Selected Block:</span>
          <span className="text-blue-400 truncate max-w-[120px]">
            {selectedNodeId || 'None'}
          </span>
        </div>
        <div className="flex justify-between">
          <span className="text-slate-500">Solver Mode:</span>
          <span className="text-emerald-500 font-bold">Ode45 (Dormand-Prince)</span>
        </div>
        <div className="flex justify-between">
          <span className="text-slate-500">Step Size:</span>
          <span className="text-slate-400">Fixed (50µs)</span>
        </div>
      </div>
    </div>
  );
}
