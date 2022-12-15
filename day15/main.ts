import { readFileSync } from "fs";

if(process.argv.length != 3) {
    console.log("Need a input file as only argument");
    process.exit(); 
}

const raw = readFileSync(process.argv[2], { encoding: "utf-8" })
    .split("\n")
    .filter(str => str.trim().length != 0);

interface Position {
    x : number;
    y : number;
}

interface SensorData {
    sensor : Position;
    beacon : Position;
}

function computeManhattan(lhs: Position, rhs:Position) : number{
    return Math.abs(lhs.x- rhs.x) + Math.abs(lhs.y-rhs.y);
}

const parsed = raw.map(line => 
    line
        .split(" ")
        .filter(word => word.startsWith("x") || word.startsWith("y"))
        .map(word => word.replaceAll(/[^0-9-]/g, ""))
        .map(word => parseInt(word, 10))
        
).map(numbers => ({ 
    sensor : { x : numbers[0], y : numbers[1] }, beacon : { x : numbers[2], y : numbers[3] } 
})) satisfies SensorData[];

// const row = 10;
const row =2000000; 
const flattenToRow = parsed.map(sensorData => {
    const distance = computeManhattan(sensorData.sensor, sensorData.beacon) - Math.abs(sensorData.sensor.y - row);
    return { x : sensorData.sensor.x , y : row, distance };
}).filter(({  distance, ...rest }) => distance >= 0);

const rangePoints= flattenToRow.flatMap(data => {
    if(data.distance == 0) return data.x;
    return [ data.x - data.distance, data.x + data.distance ];
});

const leftMost= Math.min(...rangePoints);
const rightMost= Math.max(...rangePoints);

console.log(parsed);
console.log(flattenToRow);
console.log(leftMost);
console.log(rightMost);
console.log(rightMost - leftMost)
