'use client';

import React, { useEffect, useMemo, useRef } from 'react';
import ReactFlow, {
  Background,
  NodeTypes,
  useNodesState,
  useEdgesState,
  Handle,
  Position,
  NodeProps,
  useReactFlow,
  Panel,
} from 'reactflow';
import 'reactflow/dist/style.css';
import { initialNodes, initialEdges, ElectricalNodeData } from '@/lib/model-definition';
import { Cpu, Eye, Radio, Sun, Zap } from 'lucide-react';

const CustomElectricalNode = ({ id, data, selected }: NodeProps<ElectricalNodeData>) => {
  const getNodeStyling = () => {
    if (id === 'grid-source') return 'border-cyan-400/80 bg-[#061826] text-cyan-300';
    if (id === 'series-trans') return 'border-cyan-500/70 bg-[#061826] text-cyan-200';
    if (id === 'series-inv') return 'border-emerald-500/80 bg-[#051c14] text-emerald-300';
    if (id === 'shunt-inv') return 'border-purple-500/80 bg-[#160c24] text-purple-300';
    if (id === 'dc-link') return 'border-amber-500 bg-[#291705] text-amber-300 font-bold';
    if (id === 'microgrid') return 'border-amber-400 bg-[#231503] text-amber-200';
    if (id === 'critical-load') return 'border-emerald-500/80 bg-[#051c14] text-emerald-300';
    if (id === 'diode-rectifier') return 'border-rose-500/80 bg-[#240a11] text-rose-300';
    if (id === 'scope-block') return 'border-cyan-400 bg-[#051522] text-cyan-400';
    return 'border-slate-700 bg-[#0b101b] text-slate-300';
  };

  const renderIcon = () => {
    if (id === 'grid-source') return <Radio className="w-5 h-5 text-cyan-400" />;
    if (id === 'series-trans') return <span className="text-sm font-bold text-cyan-400">∞</span>;
    if (id === 'series-inv' || id === 'shunt-inv') return <Zap className="w-5 h-5" />;
    if (id === 'microgrid') return <Sun className="w-5 h-5 text-amber-400" />;
    if (id === 'scope-block') return <Eye className="w-5 h-5 animate-pulse text-cyan-400" />;
    return <Cpu className="w-5 h-5" />;
  };

  return (
    <div className={`p-2 rounded border-2 shadow-lg transition-all font-mono select-none ${getNodeStyling()} ${selected ? 'ring-2 ring-cyan-400 scale-105' : ''} min-w-[140px]`}>
      <Handle type="target" position={Position.Left} id="l" style={{ background: '#00f0ff', width: '6px', height: '6px' }} />
      <Handle type="source" position={Position.Right} id="r" style={{ background: '#00f0ff', width: '6px', height: '6px' }} />
      <Handle type="target" position={Position.Top} id="t" style={{ background: '#f59e0b', width: '6px', height: '6px' }} />
      <Handle type="source" position={Position.Bottom} id="b" style={{ background: '#f59e0b', width: '6px', height: '6px' }} />

      <div className="flex items-center space-x-2">
        <div className="flex-shrink-0">{renderIcon()}</div>
        <div>
          <div className="text-[10px] font-bold tracking-wide uppercase leading-tight">{data.label}</div>
          {data.details && (
            <div className="text-[8px] opacity-80 whitespace-pre-line leading-none mt-0.5">{data.details}</div>
          )}
          {data.parameters && (
            <div className="text-[7.5px] opacity-90 mt-1 font-bold">
              {Object.entries(data.parameters).map(([k, v]) => `${k}:${v}`).join(' ')}
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

interface SimulationCanvasProps {
  isSimulating: boolean;
  selectedNodeId: string | null;
  onSelectNode: (nodeId: string | null) => void;
  setZoomInRef: React.MutableRefObject<(() => void) | null>;
  setZoomOutRef: React.MutableRefObject<(() => void) | null>;
  setFitViewRef: React.MutableRefObject<(() => void) | null>;
}

export default function SimulationCanvas({
  isSimulating,
  selectedNodeId,
  onSelectNode,
  setZoomInRef,
  setZoomOutRef,
  setFitViewRef,
}: SimulationCanvasProps) {
  const [nodes, setNodes, onNodesChange] = useNodesState(initialNodes);
  const [edges, setEdges, onEdgesChange] = useEdgesState(initialEdges);
  const { zoomIn, zoomOut, fitView } = useReactFlow();

  useEffect(() => {
    setZoomInRef.current = () => zoomIn({ duration: 300 });
    setZoomOutRef.current = () => zoomOut({ duration: 300 });
    setFitViewRef.current = () => fitView({ duration: 300, padding: 0.1 });
  }, [zoomIn, zoomOut, fitView, setZoomInRef, setZoomOutRef, setFitViewRef]);

  const nodeTypes = useMemo<NodeTypes>(
    () => ({
      electrical: CustomElectricalNode,
      control: CustomElectricalNode,
      microgrid: CustomElectricalNode,
      scope: CustomElectricalNode,
    }),
    []
  );

  return (
    <div className="flex-1 h-full bg-[#050912] relative overflow-hidden select-none">
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onNodeClick={(event, node) => onSelectNode(node.id)}
        onPaneClick={() => onSelectNode(null)}
        nodeTypes={nodeTypes}
        fitView
        fitViewOptions={{ padding: 0.1 }}
      >
        <Background color="#0f2238" gap={18} size={1} />

        {/* Canvas HUD matching exact screenshot design */}
        <Panel position="top-left" className="m-3 p-3 bg-[#081220]/95 text-slate-100 rounded-md border border-cyan-900/60 font-mono text-[10px] space-y-1 shadow-2xl max-w-[210px]">
          <div className="font-bold text-cyan-400 uppercase border-b border-cyan-900/50 pb-1 mb-1 tracking-wider text-[9px]">
            CANVAS HUD
          </div>
          <div>Grid Base <span className="text-cyan-300 font-bold">415V LL</span></div>
          <div>Frequency <span className="text-cyan-300 font-bold">50 Hz</span></div>
          <div>Solar Array <span className="text-amber-400 font-bold">5×305W / 1.525kW</span></div>
          <div>Mode <span className="text-emerald-400 font-bold">Active Compensation</span></div>
        </Panel>
      </ReactFlow>
    </div>
  );
}
