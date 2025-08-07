const { performance } = require('perf_hooks');

function printMemoryUsageCPU(message) {
    const memoryUsage = process.memoryUsage();
    const rssMB = memoryUsage.rss / 1024 / 1024; // Convert from bytes to MB
    console.log(`${message} - CPU Memory Usage: ${rssMB.toFixed(2)} MB`);
}


const start = performance.now(); // Start time

const gpjson = Polyglot.eval('gpjson', 'jsonpath');
const result = gpjson.query('../../dataset/bestbuy_small_records.json', '$.shippingLevelsOfService.test.test.test.test.test.test.test');

const end = performance.now(); // End time

console.log(`Execution time: ${end - start} ms`); // Log the execution time

console.log("Query Result:", result[0][0][0]);
// console.log("Query Result_3:", JSON.stringify(result, null, 1));

