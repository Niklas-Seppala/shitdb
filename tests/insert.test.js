const { fail } = require("assert");
const { executeCommands, parseRow } = require("./bridge.js");

function getData(n) {
  const data = [];
  for (let i = 0; i < n; i++) {
    data.push({
      id: i,
      username: `name-${i}`,
      email: `email-${i}@domain.com`,
    });
  }
  return data;
}

function getCommands(data) {
  const commands = [];
  for (let i = 0; i < data.length; i++) {
    commands.push(`insert ${data[i].id} ${data[i].username} ${data[i].email}`);
  }
  commands.push("select");
  commands.push(".exit");
  return commands;
}

const amount = 1400;
test(`Inserts ${amount} rows and queries the table`, async () => {
  const data = getData(amount);
  const commands = getCommands(data);
  await executeCommands(commands)
    .then(output =>
      output.forEach((json, i) =>
        parseRow(json, row => {
          expect(row.id).toBe(data[i].id);
          expect(row.username).toBe(data[i].username);
          expect(row.email).toBe(data[i].email);
        })
      )
    )
    .catch(error => fail(error.message));
});

test(`Insert max sized entry (31 bytes) for username column`, async () => {
  const longUsername = "a".repeat(31);
  const longEmail = "email";
  const commands = [`insert 1 ${longUsername} ${longEmail}`, "select", ".exit"];
  await executeCommands(commands)
    .then(output =>
      output.forEach(json =>
        parseRow(json, row => {
          expect(row.id).toBe(1);
          expect(row.username).toBe(longUsername);
          expect(row.email).toBe(longEmail);
        })
      )
    )
    .catch(error => fail(error.message));
});

test(`Insert max sized entry (255 bytes) for email column`, async () => {
  const longUsername = "name";
  const longEmail = "e".repeat(255);
  const commands = [`insert 1 ${longUsername} ${longEmail}`, "select", ".exit"];
  await executeCommands(commands)
    .then(output =>
      output.forEach(json =>
        parseRow(json, row => {
          expect(row.id).toBe(1);
          expect(row.username).toBe(longUsername);
          expect(row.email).toBe(longEmail);
        })
      )
    )
    .catch(error => fail(error.message));
});
