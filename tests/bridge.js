const { fail } = require("assert");
const { spawn } = require("child_process");

/**
 * @param {[string]} commands 
 * @returns {Promise<[string]>} JSON rows
 */
function executeCommands(commands) {
  return new Promise((resolve, reject) => {
    let rawOutput = "";
    const child = spawn("../out/squeel");

    commands.forEach(command => {
      child.stdin.write(`${command}\n`);
    });
    child.stdin.end();

    child.stdout.on("data", data => {
      rawOutput += data.toString();
    });

    child.on("error", err => {
      console.error(err);
      reject(err);
    });

    // Resolve when the process exits and all output has been read
    child.on("close", code => {
      if (code !== 0) {
        reject(new Error(`Process exited with code ${code}`));
      } else {
        resolve(
          rawOutput
            .replaceAll("squeel-db > ", "")
            .replaceAll("Goodbye", "")
            .split("\n")
            .filter(it => it.length > 0)
        );
      }
    });
  });
}

/**
 * @param {string} jsonRow 
 * @param {({ id: number, username: string, email: string}) => void} assert
 */
const parseRow = (jsonRow, assert) => {
  let row;
  try {
    row = JSON.parse(jsonRow);
  } catch (e) {
    fail(`Failed to parse row "${jsonRow}"`);
  }
  assert(row);
};

module.exports = { executeCommands, parseRow };
