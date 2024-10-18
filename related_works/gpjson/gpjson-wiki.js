// const gpjson = Polyglot.eval('gpjson', 'jsonpath');
// const result = gpjson.query('dataset.json', '$.foo');


// const gpjson = Polyglot.eval('gpjson', 'jsonpath');

// Measure time for evaluation
console.time('Evaluation Time');
const gpjson = Polyglot.eval('gpjson', 'jsonpath');
console.timeEnd('Evaluation Time');

// Measure time for query
console.time('Query Time');
const result = gpjson.query('/home/csgrads/aveda002/Desktop/CUDA-Test/JSONPARSING/Test-Files/Pison Large Datasets/wiki_small_records_remove.json', '$.type');
console.timeEnd('Query Time');

// Print message
console.log(result);



