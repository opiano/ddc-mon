const fs = require('fs');
const path = require('path');
const dir = 'd:/temp/github-opiano/ddc-mon/bacnet-web/src/views';
const files = fs.readdirSync(dir).filter(f => f.endsWith('.vue'));

files.forEach(file => {
  const p = path.join(dir, file);
  let content = fs.readFileSync(p, 'utf8');
  content = content.replace(/<th-2>/g, '<th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">');
  content = content.replace(/<td-1>/g, '<td class="text-sm text-slate-400 font-mono py-1 pl-4">');
  fs.writeFileSync(p, content);
});
console.log('Fixed tags.');
