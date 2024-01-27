/* eslint-disable max-depth */
import fs from 'fs';

const STATE = {
    INITIAL: 'INITIAL',
    STABLE: 'STABLE',
    SOFT_STABLE: 'SOFT_STABLE',
    UNSTABLE: 'UNSTABLE',
};

const MAX_LENGTH = 50;

const run = () => {
    const rawTable = fs
        .readFileSync(
            '<insert path>'
        )
        .toString();

    const [_, ...arrayTable] = rawTable.split('\r\n');

    const table = arrayTable.map((row) => {
        const [time, value] = row.split(',');

        return {time: Number(time), value: Number(value)};
    });

    const queue = [];
    const softStableValues = [];
    let prevState = STATE.INITIAL;
    let prevVesselWeight = 0;

    for (let index = 0; index < table.length; index++) {
        const entry = table[index];

        if (queue.length === MAX_LENGTH) {
            let maxDiff = 0;

            for (let j = 0; j < queue.length - 1; j++) {
                const queueElement = queue[j];
                const nextQueueElement = queue[j + 1];

                const diff = Math.abs(queueElement.value - nextQueueElement.value);

                if (maxDiff < diff) {
                    maxDiff = diff;
                }
            }

            let state = STATE.INITIAL;
            const sum = queue.reduce((acc, v) => acc + v.value, 0);
            const average = sum / queue.length;

            // console.log('maxDiff', maxDiff);

            if (maxDiff < 20) {
                state = STATE.STABLE;
            }

            if (maxDiff >= 20 && maxDiff < 900) {
                state = STATE.SOFT_STABLE;
            }

            if (maxDiff >= 900) {
                state = STATE.UNSTABLE;
            }

            if (prevState !== state) {
                if (state === STATE.STABLE) {
                    if (prevState !== STATE.INITIAL) {
                        const vesselDiff = average - prevVesselWeight;
                        if (vesselDiff > 100) {
                            const catWeight = Math.max(...softStableValues) - average;
                            console.log('cat pooped', vesselDiff, catWeight);
                            // report diff
                            // report cat weight
                            // increae poo count
                        } else if (vesselDiff < 100) {
                            console.log('cleaning success', vesselDiff);
                            // drop poo count
                        }

                        softStableValues.splice(0, softStableValues.length);
                    }
                }

                if (state === STATE.SOFT_STABLE) {
                    softStableValues.push(average);
                }
                console.log(state);
                console.log(average);
                prevState = state;
            }

            if (state === STATE.STABLE) {
                prevVesselWeight = average;
            }

            queue.shift();
        }

        queue.push(entry);
    }
};

run();
