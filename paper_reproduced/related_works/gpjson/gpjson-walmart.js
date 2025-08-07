const { performance } = require('perf_hooks');
// const util = require('util'); // Import the util module for custom inspection

function printMemoryUsageCPU(message) {
    const memoryUsage = process.memoryUsage();
    const rssMB = memoryUsage.rss / 1024 / 1024; // Convert from bytes to MB
    console.log(`${message} - CPU Memory Usage: ${rssMB.toFixed(2)} MB`);
}

const start = performance.now(); // Start time

const gpjson = Polyglot.eval('gpjson', 'jsonpath');
const result = gpjson.query('../../dataset/walmart_small_records.json', '$.items.name.test.test.test.test.test.test.test');

const end = performance.now(); // End time


console.log(`Execution time: ${end - start} ms`); // Log the execution time


// console.log("test 2");

printMemoryUsageCPU("1 - ");


// const gpjson = Polyglot.eval('gpjson', 'jsonpath');
// const result = gpjson.query('dataset.json', '$.value');
// printMemoryUsageCPU("1 - ");

console.log("Query Result:", result[0][0][0]);
// console.log("Query Result_3:", JSON.stringify(result, null, 1));

// const { readFileSync } = require('fs');
// const Polyglot = require('node-polyglot');

// const gpjson = Polyglot.eval('gpjson', 'jsonpath');

// // Function to read JSON file
// function readJsonFile(filePath) {
//     return JSON.parse(readFileSync(filePath, 'utf8'));
// }

// // Function to query JSON data
// function queryJsonData(jsonFilePath, jsonPath) {
//     const jsonData = readJsonFile(jsonFilePath);
//     return gpjson.query(jsonData, jsonPath);
// }

// // Example usage
// const jsonFilePath = './Test-Files/test-simple-rapid.json'; // Replace with your JSON file path
// const jsonPath = '$.name'; // Replace with your JSONPath query

// try {
//     const result = queryJsonData(jsonFilePath, jsonPath);
//     console.log('Query result:', result);
// } catch (error) {
//     console.error('Error querying JSON data:', error);
// }
