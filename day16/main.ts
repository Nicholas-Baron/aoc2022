import { readFileSync } from "fs";

if(process.argv.length != 3) {
    console.log("Need a input file as only argument");
    process.exit(); 
}

const raw = readFileSync(process.argv[2], { encoding: "utf-8" })
    .split("\n")
    .filter(str => str.trim().length != 0);

type NodeInfo = { flow_rate: number, neighbors: Array<string> };
const graph = new Map<string, NodeInfo>();
for (const line of raw){
    const words = line.split(" ");
    const node = words[1];
    const flow_rate = parseInt(words
        .filter(word => word.includes("="))
        .map(word => word.replaceAll(/[^0-9]/g, ""))[0]
    , 10);
    const neighbors = words
        .filter(word => /[A-Z][A-Z]/.test(word))
        .map(word => word.replaceAll(",",""))
        .slice(1);
    graph.set(node, { flow_rate, neighbors });
}


type DistanceMatrix = Map<string, number>;

function floydWarshalls(graph: Map<string, NodeInfo>) : DistanceMatrix {
    const distances = new Map();

    // Initialization
    for (const source of graph.keys()){
        const info = graph.get(source);
        if (info === undefined) continue;
        for (const dest of info.neighbors){
            distances.set(source+ dest, 1);
            distances.set(dest+ source, 1);
        }
    }

    for (const intermediate of graph.keys()){
        for (const source of graph.keys()){
            if(intermediate === source) continue;
            for (const dest of graph.keys()){
                if(intermediate === dest || source === dest) continue;

                const sourceToInter = distances.get(source + intermediate);
                const interToDest = distances.get(intermediate + dest);

                // Infinity case
                if(sourceToInter === undefined || interToDest === undefined) continue;

                const currentDistance = distances.get(source + dest);

                if (currentDistance === undefined || currentDistance > sourceToInter + interToDest){
                    distances.set(source + dest, sourceToInter + interToDest);
                }
            }
        }
    }

    return distances;
}


const distances = floydWarshalls(graph);
const nodes = Array.from(graph.keys());


type PathNode = {
    node: string;
    relief: number;
    distance: number;
    openAt: number;
}
type Path = Array<PathNode>


function buildPaths(timeRemaining: number, currentNode: string, openValves: Set<string>): Array<Path> {
    if(timeRemaining < 0) return []

    const distanceTo = (node: string) => distances.get(currentNode + node) ?? Number.MAX_SAFE_INTEGER-1;

    const reachableNodes = nodes.filter(node => 
        (graph.get(node)?.flow_rate ?? 0) != 0 
        && !openValves.has(node)
        && distanceTo(node) + 1 < timeRemaining
    );
    if(reachableNodes.length === 0) return [];
    if(reachableNodes.length === 1){
        const distance = distanceTo(reachableNodes[0]);
        const timeRemainingAfterOpening = timeRemaining - (distance + 1);
        const relief = timeRemainingAfterOpening * (graph.get(reachableNodes[0])?.flow_rate ?? 0);

        return [
            [
                {
                    node:reachableNodes[0],
                    relief: relief,
                    distance: distance,
                    openAt: 30 - timeRemainingAfterOpening,
                }
            ]
        ];
    }

    let result = new Array<Path>();
    for (const node of reachableNodes){
        const distance = distanceTo(node);
        const timeRemainingAfterOpening = timeRemaining - (distance + 1);
        const relief = timeRemainingAfterOpening * (graph.get(node)?.flow_rate ?? 0);

        openValves.add(node);
        const childPaths = buildPaths(timeRemainingAfterOpening, node, openValves);
        openValves.delete(node);

        for (const path of childPaths) path.unshift({ node:node, distance:distance, relief:relief, openAt: (30 - timeRemainingAfterOpening) });

        result = result.concat(childPaths);
    }
    return result;
}

const paths = buildPaths(30, "AA", new Set());

const computeRelief = (path: Path) => path.map(node => node.relief).reduce( (acc, val) => acc + val );

const bestPath = paths.reduce((currentBest : [number, Path], path):[number,Path] => {
    const relief = computeRelief(path);
    return relief > currentBest[0]
        ? [relief, path]
        : currentBest;
    
}, [0, new Array()]);

console.log(distances)
// console.log(paths.filter(path => path[0].node == 'DD'))
console.log(bestPath);

