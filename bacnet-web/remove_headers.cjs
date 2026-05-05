const fs = require('fs');
const path = require('path');
const dir = 'd:/temp/github-opiano/ddc-mon/bacnet-web/src/views';
const files = ['AnalogInputs.vue', 'AnalogOutputs.vue', 'AnalogValues.vue', 'BinaryInputs.vue', 'BinaryOutputs.vue', 'BinaryValues.vue', 'MultiStateValues.vue'];

files.forEach(file => {
  const p = path.join(dir, file);
  if (!fs.existsSync(p)) return;
  let content = fs.readFileSync(p, 'utf8');

  // Change py-4 to py-2 for the header
  content = content.replace(/<div class="px-6 py-4 flex items-center justify-between/g, '<div class="px-6 py-2 flex items-center justify-between');

  // Remove the buttons div
  const regex = /<div class="flex items-center gap-3">\s*<button class="bg-surface-container-high[^>]*>[\s\S]*?<\/button>\s*<button class="bg-primary[^>]*>[\s\S]*?<\/button>\s*<\/div>/g;
  content = content.replace(regex, '');

  fs.writeFileSync(p, content);
});
console.log('Fixed headers in views.');
