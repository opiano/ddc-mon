const fs = require('fs');
const path = require('path');
const dir = 'd:/temp/github-opiano/ddc-mon/bacnet-web/src/views';
const files = ['AnalogInputs.vue', 'AnalogOutputs.vue', 'AnalogValues.vue', 'BinaryInputs.vue', 'BinaryOutputs.vue', 'BinaryValues.vue', 'MultiStateValues.vue'];

files.forEach(file => {
  const p = path.join(dir, file);
  if (!fs.existsSync(p)) return;
  let content = fs.readFileSync(p, 'utf8');

  const regex = /<div class="flex gap-2">\s*<button[\s\S]*?Filter\s*<\/button>\s*<button[\s\S]*?New Object\s*<\/button>\s*<\/div>/g;
  content = content.replace(regex, '');

  fs.writeFileSync(p, content);
});
console.log('Fixed buttons.');
