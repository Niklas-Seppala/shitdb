const { fail } = require("assert");
const { spawn } = require("child_process");
const { rmSync, existsSync, mkdirSync, unlinkSync } = require("fs")

const dbDir = "out/test/"

/**
 * @param {string} suiteName
 * @param {string} caseName
 * @returns {string} dir
 */
function getDbPath(suiteName, caseName) {
  const dbPath = dbDir + suiteName + "/" + caseName + ".db";
  if (existsSync(dbPath)) {
    unlinkSync(dbPath);
  } else {
    mkdirSync(dbDir + suiteName, {recursive: true});
  }
  return dbPath;
}

/**
 * @param {[string]} commands 
 * @param {string|null} file
 * @returns {Promise<[string]>} JSON rows
 */
function executeCommands(commands, file) {
  return new Promise((resolve, reject) => {
    let rawOutput = "";
    let rawError = "";
    const child = spawn("../out/sdb", [file]);

    commands.forEach(command => {
      child.stdin.write(`${command}\n`);
    });
    child.stdin.end();

    child.stdout.on("data", data => {
      rawOutput += data.toString();
    });

    child.stderr.on("data", data => {
      rawError += data.toString();
    });

    child.on("error", err => {
      console.error(err);
      reject(err);
    });

    // Resolve when the process exits and all output has been read
    child.on("close", code => {
      if (code !== 0) {
        console.error("---------- error ----------")
        console.error(rawError)
        console.log("---------- OUTPUT ----------")
        console.log(rawOutput);
        reject(new Error(`Process exited with code ${code}`));
      } else {
        resolve(
          rawOutput
            .replaceAll("sdb > ", "")
            .replaceAll("Goodbye", "")
            .split("\n")
            .filter(it => it.length > 0)
            .filter(it => !it.includes("[CREATE]"))
            .filter(it => !it.includes("[OPEN]"))
            .filter(it => !it.includes("[ERROR]"))
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

module.exports = { executeCommands, parseRow, getDbPath };
