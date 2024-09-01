const { fail } = require("assert");
const { executeCommands, parseRow, getDbPath } = require("./bridge.js");

test(`Insert row into database and expect it to persist between startups`, async () => {
  const dbPath = getDbPath("persistence", "row");
  const username = "name";
  const email = "email";
  const id = 1;
  const commands = [`insert id=${id} username=${username} email=${email}`, "select", ".exit"];
  await executeCommands(commands, dbPath)
    .then(output => {
      expect(output.length).toBe(1);
      output.forEach(json =>
        parseRow(json, row => {
          expect(row.id).toBe(id);
          expect(row.username).toBe(username);
          expect(row.email).toBe(email);
        })
      );
    })
    .catch(error => fail(error.message));

  await executeCommands(["select", ".exit"], dbPath)
    .then(output => {
      expect(output.length).toBe(1);
      output.forEach(json =>
        parseRow(json, row => {
          expect(row.id).toBe(id);
          expect(row.username).toBe(username);
          expect(row.email).toBe(email);
        })
      );
    })
    .catch(error => fail(error.message));
});
