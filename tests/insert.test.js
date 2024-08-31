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
    commands.push(`insert id=${data[i].id} username=${data[i].username} email=${data[i].email}`);
  }
  commands.push("select");
  commands.push(".exit");
  return commands;
}
/*
==14599== 
squeel-db > INSERT id=0 username=name-0 email=email-0@domain.com
==14599== Conditional jump or move depends on uninitialised value(s)
==14599==    at 0x4853CD3: strcspn (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==14599==    by 0x491C391: strtok_r (strtok_r.c:64)
==14599==    by 0x10A2E3: tokenize_insert (tokenizer.c:47)
==14599==    by 0x10A38B: tokenize (tokenizer.c:94)
==14599==    by 0x1098EE: squeel_statement_prepare (statement.c:18)
==14599==    by 0x1096F8: main (main.c:32)
==14599== 
==14599== Conditional jump or move depends on uninitialised value(s)
==14599==    at 0x491C399: strtok_r (strtok_r.c:65)
==14599==    by 0x10A2E3: tokenize_insert (tokenizer.c:47)
==14599==    by 0x10A38B: tokenize (tokenizer.c:94)
==14599==    by 0x1098EE: squeel_statement_prepare (statement.c:18)
==14599==    by 0x1096F8: main (main.c:32)
==14599== 
==14599== Conditional jump or move depends on uninitialised value(s)
==14599==    at 0x484F02A: strncpy (in /usr/libexec/valgrind/vgpreload_memcheck-amd64-linux.so)
==14599==    by 0x10A29F: tokenize_insert (tokenizer.c:62)
==14599==    by 0x10A38B: tokenize (tokenizer.c:94)
==14599==    by 0x1098EE: squeel_statement_prepare (statement.c:18)
==14599==    by 0x1096F8: main (main.c:32)
==14599== 
==14599== Conditional jump or move depends on uninitialised value(s)
==14599==    at 0x491C36F: strtok_r (strtok_r.c:49)
==14599==    by 0x10A2E3: tokenize_insert (tokenizer.c:47)
==14599==    by 0x10A38B: tokenize (tokenizer.c:94)
==14599==    by 0x1098EE: squeel_statement_prepare (statement.c:18)
==14599==    by 0x1096F8: main (main.c:32)
==14599== 
*/
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
  const longUsername = "a".repeat(30);
  const longEmail = "email";
  const commands = [`insert id=1 username=${longUsername} email=${longEmail}`, "select", ".exit"];
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
  const commands = [`insert id=1 username=${longUsername} email=${longEmail}`, "select", ".exit"];
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
