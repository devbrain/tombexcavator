// Copyright (C) 2005 Crestez Leonard    cleonard@go.ro
//
// This file is part of Alpha
//
// Alpha is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Alpha is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Alpha; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.IO;
using System.Collections;

using Alpha.Util;

namespace Alpha.Game
{
    // This is a class that can pretty-print animations to html.
    // Usefull for debugging.
    public class ScriptPrettyPrinter
    {
        // Title of the html to print.
        public string Title = "Script listing";

        // Hashtable of named nodes.
        // maps nodes to names.
        public readonly Hashtable NamedNodes;

        // Names of the animations. This should be equal in size to the EntryPoints array.
        public string[] AnimationNames;

        // Entry points for the animations.
        public ScriptNode[][] EntryPoints;

        // This fills out any missing animation names as anim-{id}
        private void GenerateAnimationNames()
        {
            int i;
            if (AnimationNames == null || AnimationNames.Length != EntryPoints.Length) {
                AnimationNames = new string[EntryPoints.Length];
            }
            for (i = 0; i < EntryPoints.Length; ++i) {
                if (AnimationNames[i] == null) {
                    AnimationNames[i] = "anim-" + i;
                }
            }
        }

        // Generates names for entry points.
        // They are in the form of {AnimationName}-{id}
        // AnimationNames are supposed to be valid.
        private void GenerateEntryPointNames()
        {
            int i, j;
            for (i = 0; i < EntryPoints.Length; ++i) {
                if (EntryPoints[i] == null) {
                    throw new Exception("null entry points");
                }
                for (j = 0; j < EntryPoints[i].Length; ++j) {
                    ScriptNode node = EntryPoints[i][j];
                    if (!NamedNodes.Contains(node)) {
                        NamedNodes.Add(node, AnimationNames[i] + "-" + j);
                    }
                }
            }
        }

        // This does a preliminary visit of the nodes and generates
        // any missing names.
        private void VisitNodes()
        {
            Queue q = new Queue();
            Collection vis = new Collection();
            int i, j;
            int k = 0;

            for (i = 0; i < EntryPoints.Length; ++i) {
                for (j = 0; j < EntryPoints[i].Length; ++j) {
                    // the same entry point mighy be reference from several locations.
                    if (!vis.Contains(EntryPoints[i][j])) {
                        q.Enqueue(EntryPoints[i][j]);
                        vis.Add(EntryPoints[i][j]);
                    }
                }
            }
            while (q.Count > 0) {
                ScriptNode node = q.Dequeue() as ScriptNode;
                if (node == null) {
                    continue;
                }
                if (!vis.Contains(node.Next)) {
                    q.Enqueue(node.Next);
                    vis.Add(node.Next);
                } else {
                    if (!NamedNodes.Contains(node.Next)) {
                        NamedNodes.Add(node.Next, "node-" + ++k);
                    }
                }
            }
        }

        // This generates all needed names:
        // AnimationNames
        // names for EntryPoints
        // names for nodes referenced from more than one location.
        //
        // This will not override any existing names.
        public void GenerateNames()
        {
            GenerateAnimationNames();
            GenerateEntryPointNames();
            VisitNodes();
        }

        // This function sets the names for individual entry points of
        // a certain anim. the names are formatted, {0} being AnimationNames[i].
        public void SetEntryPointNames(int anim, string[] names)
        {
            int i;
            if (names.Length != EntryPoints[anim].Length) {
                return;
            }
            for (i = 0; i < names.Length; ++i) {
                NamedNodes[EntryPoints[anim][i]] = String.Format(names[i], AnimationNames[anim]);
            }
        }

        // Returns a html reference to a node.
        // If the node doesn't have an id, it throws an exception.
        private string NodeReference(ScriptNode node)
        {
            string name = NamedNodes[node] as string;
            if (name == null) {
                throw new Exception("Bad node reference, not found");
            }
            return String.Format("<a href=\"#{0}\">{0}</a>", name);
        }

        // Returns a html ancher for a node label.
        private string NodeLabel(ScriptNode node)
        {
            string name = NamedNodes[node] as string;
            if (name == null) {
                throw new Exception("Bad node reference, not found");
            }
            return String.Format("<a name=\"{0}\">{0}</a>", name);
        }

        // Format a script node as a string.
        private string NodeFormat(ScriptNode node)
        {
            if (node is ScriptNodeWait) {
                return String.Format("Wait {0} cycles", ((ScriptNodeWait)node).WaitTime);
            } else if (node is ScriptNodeChangeFrame) {
                return String.Format("Show frame {0}", ((ScriptNodeChangeFrame)node).NewFrame);
            } else if (node is ScriptNodeBreakAction) {
                return "Break point";
            } else if (node is ScriptNodeMove) {
                return String.Format("Move {0} entities", ((ScriptNodeMove)node).MoveDistance);
            } else if (node is ScriptNodeAction) {
                return "Action hot-spot";
            } else if (node.GetType() == typeof(ScriptNode)) {
                return "Nothing / Jump";
            } else {
                return "Unknown: " + node.ToString();
            }
        }

        // Prints the header, containing a list of animations and their entry points.
        private void PrintHeader(TextWriter writer)
        {
            int i, j;
            writer.Write("<table border=\"1\"><tr><th>Animation</th><th colspan=\"100\">Entry points</th></tr>");
            for (i = 0; i < EntryPoints.Length; ++i) {
                writer.Write("<tr><td>{0}</td>", AnimationNames[i]);
                for (j = 0; j < EntryPoints[i].Length; ++j) {
                    writer.Write("<td>{0}</td>", NodeReference(EntryPoints[i][j]));
                }
                writer.WriteLine("</tr>");
            }
            writer.Write("</table>");
        }

        private void PrintAnimations(TextWriter writer)
        {
            int i, j;
            Collection visited = new Collection();
            ScriptNode node;

            writer.WriteLine("<table border=\"1\"><tr><th>Label</th><th>Instructions</th></tr>");
            for (i = 0; i < EntryPoints.Length; ++i) {
                for (j = 0; j < EntryPoints[i].Length; ++j) {
                    node = EntryPoints[i][j];
                    if (visited.Contains(node)) {
                        continue;
                    }
                    writer.Write("<tr></tr>");
                    while (!visited.Contains(node)) {
                        writer.Write("<tr><td>");
                        if (NamedNodes.Contains(node)) {
                            writer.Write("{0}", NodeLabel(node));
                        }
                        writer.WriteLine("</td><td>{0}</td></tr>", NodeFormat(node));
                        visited.Add(node);
                        node = node.Next;
                    }
                    writer.WriteLine("<tr><td></td><td>Jump to {0}</td></tr>", NodeReference(node));
                    writer.WriteLine("<tr></tr>");
                }
            }
            writer.WriteLine("</table>");
 
        }

        // Print everything to a writer.
        public void Print(TextWriter writer)
        {
            writer.Write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" ");
            writer.Write("\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">");
            writer.Write("<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">");
            writer.WriteLine("<head><title>{0}</title></head><body>", Title);
            GenerateNames();
            PrintHeader(writer);
            writer.Write("<br />");
            PrintAnimations(writer);
            writer.Write("</body></html>");
        }

        // Print to a file.
        public void Print(string filename)
        {
            FileStream stream = new FileStream(filename, FileMode.Create, FileAccess.ReadWrite);
            TextWriter writer = new StreamWriter(stream);
            try {
                Print(writer);
            } finally {
                writer.Close();
            }
        }

        public ScriptPrettyPrinter()
        {
            NamedNodes = new Hashtable();
        }
    }
}

