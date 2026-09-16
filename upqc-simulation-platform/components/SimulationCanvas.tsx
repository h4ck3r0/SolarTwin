'use client';

import React, { useEffect, useMemo, useRef, useCallback } from 'react';
import ReactFlow, {
  Background,
  NodeTypes,
  useNodesState,
  useEdgesState,
  Position,
  NodeProps,
  useReactFlow,
  Panel,
  Connection,
  Edge,
  addEdge,
  Handle
} from 'reactflow';
import 'reactflow/dist/style.css';
import { initialNodes, initialEdges, ElectricalNodeData } from '@/lib/model-definition';
import { Cpu, Eye, Radio, Sun, Zap } from 'lucide-react';

const CustomElectricalNode = ({ id, data, selected }: NodeProps<ElectricalNodeData>) => {
  const getNodeStyling = () => {
    if (id === 'grid-source') return 'border-sky-500 bg-white text-sky-800';
    if (id === 'series-trans') return 'border-sky-500 bg-white text-sky-700';
    if (id === 'series-inv') return 'border-emerald-500 bg-white text-emerald-800';
    if (id === 'shunt-inv') return 'border-purple-500 bg-white text-purple-800';
    if (id === 'dc-link') return 'border-amber-400 bg-white text-amber-600 font-bold';
    if (id === 'microgrid') return 'border-amber-400 bg-white text-amber-700';
    if (id === 'critical-load') return 'border-emerald-500 bg-white text-emerald-800';
    if (id === 'diode-rectifier') return 'border-rose-500 bg-white text-rose-800';
    if (id === 'scope-block') return 'border-sky-500 bg-white text-sky-700';
    return 'border-slate-300 bg-white text-slate-700';
  };

  const renderIcon = () => {
    if (id === 'grid-source') return <Radio className="w-5 h-5 text-sky-600" />;
    if (id === 'series-trans') return <span className="text-sm font-bold text-sky-600">∞</span>;
    if (id === 'series-inv' || id === 'shunt-inv') return <Zap className="w-5 h-5" />;
    if (id === 'microgrid') return <Sun className="w-5 h-5 text-amber-600" />;
    if (id === 'scope-block') return <Eye className="w-5 h-5 animate-pulse text-sky-600" />;
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

const nodeTypes: NodeTypes = {
  electrical: CustomElectricalNode,
  control: CustomElectricalNode,
  microgrid: CustomElectricalNode,
  scope: CustomElectricalNode,
};

interface SimulationCanvasProps {
  isSimulating: boolean;
  selectedNodeId: string | null;
  onSelectNode: (nodeId: string | null) => void;
  setZoomInRef: React.MutableRefObject<(() => void) | null>;
  setZoomOutRef: React.MutableRefObject<(() => void) | null>;
  setFitViewRef: React.MutableRefObject<(() => void) | null>;
  getTopologyRef: React.MutableRefObject<(() => { nodes: any[]; edges: any[] }) | null>;
  setUpdateNodeRef: React.MutableRefObject<((id: string, data: any) => void) | null>;
  onNodeDoubleClick?: (event: React.MouseEvent, node: any) => void;
}

export default function SimulationCanvas({
  isSimulating,
  selectedNodeId,
  onSelectNode,
  setZoomInRef,
  setZoomOutRef,
  setFitViewRef,
  getTopologyRef,
  setUpdateNodeRef,
  onNodeDoubleClick,
}: SimulationCanvasProps) {
  const initialNodesState = useMemo(() => {
    if (typeof window !== 'undefined') {
      const savedTopology = localStorage.getItem('solar_twin_topology');
      if (savedTopology) {
        try {
          const { nodes: savedNodes } = JSON.parse(savedTopology);
          if (savedNodes && savedNodes.length > 0) return savedNodes;
        } catch (e) {}
      }
    }
    return initialNodes;
  }, []);

  const initialEdgesState = useMemo(() => {
    if (typeof window !== 'undefined') {
      const savedTopology = localStorage.getItem('solar_twin_topology');
      if (savedTopology) {
        try {
          const { edges: savedEdges } = JSON.parse(savedTopology);
          if (savedEdges && savedEdges.length > 0) return savedEdges;
        } catch (e) {}
      }
    }
    return initialEdges;
  }, []);

  const [nodes, setNodes, onNodesChange] = useNodesState(initialNodesState);
  const [edges, setEdges, onEdgesChange] = useEdgesState(initialEdgesState);
  const { zoomIn, zoomOut, fitView, project } = useReactFlow();
  const reactFlowWrapper = useRef<HTMLDivElement>(null);

  useEffect(() => {
    localStorage.setItem('solar_twin_topology', JSON.stringify({ nodes, edges }));
  }, [nodes, edges]);

  useEffect(() => {
    setZoomInRef.current = () => zoomIn({ duration: 300 });
    setZoomOutRef.current = () => zoomOut({ duration: 300 });
    setFitViewRef.current = () => fitView({ duration: 300, padding: 0.1 });
    getTopologyRef.current = () => ({ nodes, edges });
    setUpdateNodeRef.current = (id: string, params: any) => {
      setNodes((nds) => nds.map((n) => (n.id === id ? { ...n, data: { ...n.data, parameters: params } } : n)));
    };
  }, [zoomIn, zoomOut, fitView, setZoomInRef, setZoomOutRef, setFitViewRef, getTopologyRef, setUpdateNodeRef, nodes, edges, setNodes]);

  const onConnect = useCallback(
    (params: Edge | Connection) => setEdges((eds) => addEdge(params, eds)),
    [setEdges]
  );


  const onDragOver = useCallback((event: React.DragEvent) => {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'move';
  }, []);

  const onDrop = useCallback(
    (event: React.DragEvent) => {
      event.preventDefault();

      const reactFlowBounds = reactFlowWrapper.current?.getBoundingClientRect();
      const typeData = event.dataTransfer.getData('application/reactflow');
      
      if (!typeData || !reactFlowBounds) return;

      const { type, label } = JSON.parse(typeData);

      const position = project({
        x: event.clientX - reactFlowBounds.left,
        y: event.clientY - reactFlowBounds.top,
      });

      const newNode = {
        id: `node_${new Date().getTime()}`,
        type,
        position,
        data: { label, type, details: 'Dynamically added component' },
      };

      setNodes((nds) => nds.concat(newNode));
    },
    [project, setNodes]
  );


  return (
    <div className="flex-1 h-full bg-[#F8FAFC] relative overflow-hidden select-none" ref={reactFlowWrapper} onDrop={onDrop} onDragOver={onDragOver}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        onNodeClick={(event, node) => onSelectNode(node.id)}
        onNodeDoubleClick={onNodeDoubleClick}
        onPaneClick={() => onSelectNode(null)}
        nodeTypes={nodeTypes}
        fitView
        fitViewOptions={{ padding: 0.1 }}
      >
        <Background color="#e2e8f0" gap={18} size={1} />

        {/* Canvas HUD matching exact screenshot design */}
        <Panel position="top-left" className="m-3 p-3 bg-white/95 text-slate-800 rounded-md border border-slate-200 font-mono text-[10px] space-y-1 shadow-md max-w-[210px]">
          <div className="font-bold text-sky-600 uppercase border-b border-cyan-900/50 pb-1 mb-1 tracking-wider text-[9px]">
            CANVAS HUD
          </div>
          <div>Mode <span className="text-sky-600 font-bold">OFF-GRID MICROGRID</span></div>
          <div>Solar Array <span className="text-amber-600 font-bold">7S×88P 415W (255.6kW)</span></div>
        </Panel>
      </ReactFlow>
    </div>
  );
}
