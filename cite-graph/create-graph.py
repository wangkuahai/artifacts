import graphviz
import csv

# import infomation from papers.csv and citations.csv
papers_filename="papers.csv"
citations_filename="citations.csv"
seed="9"

papers={};
citations={};

with open(papers_filename,mode="r",encoding="utf-8") as f: 
	reader = csv.DictReader(f)
	
	for row in reader:
		temp_dict={"Title":row["Title"].split(':')[0],"Year":row["Year"]}
		#temp_dict={"Title":row["Title"],"Year":row["Year"]}
		papers[row["ID"]]=temp_dict
		#print("get paper: ID=",row["ID"]," content=",temp_dict)

print(papers)

with open(citations_filename,mode="r",encoding="utf-8") as f: 
	reader = csv.DictReader(f)
	
	for row in reader:
		temp_dict={row["cite"]:row["cited"].split()}
		citations.update(temp_dict)
print(citations)
	

# draw graph
mygraph = graphviz.Digraph('citations', filename='citations.gv',node_attr={'color': 'lightblue2'},graph_attr={"compound":"true"})
#mygraph = graphviz.Digraph('citations', filename='citations.gv',node_attr={'color': 'lightblue2'},engine='twopi',graph_attr={"root":"9","overlap_scaling":"prism"})

for id,paper in papers.items():
	if id==seed : 
		mygraph.node(id,label=paper["Title"],style="filled",shape="box",fillcolor="red")
	else :
		mygraph.node(id,label=paper["Title"])
for cite,citeds in citations.items():
	for cited in citeds:
		mygraph.edge(cite,cited)

mygraph.view()
	


