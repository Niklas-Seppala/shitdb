const { fail } = require("assert");
const { executeCommands, parseRow, getDbPath } = require("./bridge.js");

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
    commands.push(`insert id=${data[i].id} username=${data[i].username} email=${data[i].email}`);
  }
  //commands.push("select");
  commands.push(".exit");
  return commands;
}

const amount = 14;
test(`Inserts ${amount} rows and queries the table`, async () => {
  const dbPath = getDbPath("insert", "max");
  const data = getData(amount);
  const commands = getCommands(data);
  await executeCommands(commands, dbPath)
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

test(`Inserts rows with 3..1 ids and expects ordered by id`, async () => {
  const dbPath = getDbPath("insert", "ordered");
  const ordered = getData(3);
  const reverse = [...ordered].reverse();
  const commands = getCommands(reverse);
  await executeCommands(commands, dbPath)
    .then(output =>
      output.forEach((json, i) =>
        parseRow(json, row => {
          expect(row.id).toBe(ordered[i].id);
          expect(row.username).toBe(ordered[i].username);
          expect(row.email).toBe(ordered[i].email);
        })
      )
    )
    .catch(error => fail(error.message));
});


test(`Insert max sized entry (31 bytes) for username column`, async () => {
  const dbPath = getDbPath("insert", "username");
  const longUsername = "a".repeat(30);
  const longEmail = "email";
  const commands = [`insert id=1 username=${longUsername} email=${longEmail}`, "select", ".exit"];
  await executeCommands(commands, dbPath)
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

test(`Insert duplicate key is rejected`, async () => {
  const dbPath = getDbPath("insert", "username");
  const username = "a".repeat(30);
  const email = "email";
  const commands = [
    `insert id=1 username=${username} email=${email}`,
    `insert id=1 username=name-2 email=email-2`,
    "select",
    ".exit"
  ];
  await executeCommands(commands, dbPath)
    .then(output =>
      output.forEach(json =>
        parseRow(json, row => {
          expect(row.id).toBe(1);
          expect(row.username).toBe(username);
          expect(row.email).toBe(email);
        })
      )
    )
    .catch(error => fail(error.message));
});


test(`Insert max sized entry (255 bytes) for email column`, async () => {
  const dbPath = getDbPath("insert", "email");
  const longUsername = "name";
  const longEmail = "e".repeat(255);
  const commands = [`insert id=1 username=${longUsername} email=${longEmail}`, "select", ".exit"];
  await executeCommands(commands, dbPath)
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
