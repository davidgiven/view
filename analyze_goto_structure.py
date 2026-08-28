#!/usr/bin/env python3
"""idom-based AST analysis to reconstruct structured statements from gotos."""
from clang.cindex import Index, CursorKind
import os
from collections import defaultdict

class GotoStmtInfo:
    __slots__ = ('spelling','target','line','column','filepath')
    def __init__(self, spelling,target,line,column,filepath):
        self.spelling=spelling;self.target=target;self.line=line;self.column=column;self.filepath=filepath
class LabelDef:
    __slots__ = ('name','line','column','filepath')
    def __init__(self,name,line,column,filepath):
        self.name=name;self.line=line;self.column=column;self.filepath=filepath

class GotoToStructuredConverter:
    def __init__(self, source_dir="src"):
        self.source_dir=source_dir;self.index=Index.create()
        self.all_gotos=[];self.all_labels=[]
        self.file_gotos=defaultdict(list);self.file_labels=defaultdict(list)
    def parse_source_files(self):
        c_files=[os.path.join(self.source_dir,f) for f in sorted(os.listdir(self.source_dir)) if f.endswith('.c')]
        for fp in c_files:
            print(f"\nParsing: {fp}");self._parse_file(fp)
        print("\n=== File Summary ===")
        for fp in sorted(self.file_gotos.keys()):
            print(f"  {fp.split('/')[-1]}: {len(self.file_gotos[fp])} gotos, {len(self.file_labels.get(fp,[]))} labels")
    def _parse_file(self, filepath):
        tu=self.index.parse(filepath, args=['-Isrc'])
        if not tu: return
        for c in tu.cursor.walk_preorder():
            self._process_cursor(filepath,c)
    def _process_cursor(self, fp, cur):
        if cur.kind==CursorKind.GOTO_STMT:
            t=None; line=cur.location.line if cur.location else 0; col=cur.location.column if cur.location else 0
            for ch in cur.get_children():
                if ch.kind==CursorKind.LABEL_REF: t=ch.spelling or ""; break
            if t:
                info=GotoStmtInfo(cur.spelling or '',t,line,col,fp)
                self.all_gotos.append(info);self.file_gotos.setdefault(fp,[]).append(info)
        elif cur.kind==CursorKind.LABEL_STMT:
            n=cur.spelling or ""; line=cur.location.line if cur.location else 0; col=cur.location.column if cur.location else 0
            info=LabelDef(n,line,col,fp);self.all_labels.append(info);self.file_labels.setdefault(fp,[]).append(info)
    def analyze_loop_patterns(self):
        print("\n=== Detecting Loop Patterns ===")
        for fp in sorted(self.file_gotos.keys()):
            print(f"\nFile: {fp.split('/')[-1]}");self._detect_loops_in_file(fp)
    def _detect_loops_in_file(self, fp):
        gotos=self.file_gotos.get(fp,[])
        if not gotos: return
        by_target=defaultdict(list)
        for g in gotos: by_target[g.target].append(g)
        for tgt, lst in by_target.items():
            if len(lst)<2: continue
            lines=sorted(g.line for g in lst)
            print(f"\n  Loop target: {tgt}\n    {len(lst)} gotos -> {lines}\n    Loop type: {self._classify(tgt)}")
    def _classify(self,t):
        lo=t.lower()
        if any(k in lo for k in ['counter','inc','dec','loop']): return "for (...);"
        if any(k in lo for k in ['while','cond','test']): return "while (...);"
        if any(k in lo for k in ['do','bottom','after']): return "do { ... } while (...);"
        return "while (1) { ... }"
    def detect_conditional_patterns(self):
        print("\n=== Detecting Conditional Patterns ===")
        for fp in sorted(self.file_gotos.keys()):
            print(f"\nFile: {fp.split('/')[-1]}");self._detect(fp)
    def _detect(self, fp):
        gotos=self.file_gotos.get(fp,[])
        from collections import defaultdict as dd
        by_line=dd(list)
        for g in gotos: by_line[g.line].append(g)
        for ln, lst in by_line.items():
            if len(lst)>=2:
                t=[g.target for g in lst]
                print(f"  Line {ln}: {len(lst)} gotos -> {t} -> if/else: {t[0]}/{t[1]}")

def main():
    c=GotoToStructuredConverter("src");c.parse_source_files();c.analyze_loop_patterns();c.detect_conditional_patterns()
if __name__=='__main__': main()
