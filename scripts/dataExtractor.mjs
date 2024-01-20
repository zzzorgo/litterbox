import fs from 'fs';

const run = () => {
    const buffer = fs.readFileSync('/Users/zzzorgo/Downloads/export2.csv');
    const data = buffer.toString();
    const sensorData = {
        w0: [],
        w1: [],
        w2: [],
        w3: [],
    };

    data
        .split('\n')
        .forEach((row, i) => {
            const rawRow = row.split(';');
            if (rawRow.length < 3) {
                return;
            }

            const record = {
                id: rawRow[0],
                timestamp: Number(rawRow[1]),
                sensorId: rawRow[2],
                value: Number(rawRow[3]),
            };

            if (!record.sensorId) {
                console.log('record.sensorId', record, i);
            }

            if (record.value < 50000 && record.value > -1000
                && (record.timestamp > 1705672289 && record.timestamp < 1705672370)
            ) {
                sensorData[record.sensorId].push(record);
            }

        });

    fs.writeFileSync('w0.csv', sensorData.w0.map(r => r.timestamp + ';' + r.value).join('\n'));
    fs.writeFileSync('w1.csv', sensorData.w1.map(r => r.timestamp + ';' + r.value).join('\n'));
    fs.writeFileSync('w2.csv', sensorData.w2.map(r => r.timestamp + ';' + r.value).join('\n'));
    fs.writeFileSync('w3.csv', sensorData.w3.map(r => r.timestamp + ';' + r.value).join('\n'));

    console.log('sensorData', sensorData.w0[0]);
};

run();