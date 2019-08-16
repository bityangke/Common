// using System;
// using System.Windows.Forms;
// using System.IO;
// using System.Collections;
// using System.Collections.Generic;
// using System.ComponentModel;
// using System.Data;
// using System.Drawing;
// using System.Linq;
// using System.Text;
// using System.Threading;
// using System.Threading.Tasks;
// using Microsoft.Win32;
// using System.Runtime.InteropServices;
// using System.Windows.Forms.DataVisualization.Charting;
// using System.Numerics;
// using System.IO.Ports;
// using System.Diagnostics;
using System.Windows.Forms;

namespace common
{

    public partial class impl
    {
        //void select_file(object sender, EventArgs e)
        public static void select_file(string init_dir, string filter, out string file_path, out string file_name)
        {

            file_path = string.Empty;
            file_name = string.Empty;

            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.InitialDirectory = init_dir;
                ofd.Filter = filter;
                ofd.RestoreDirectory = false;

                //DialogResult result = ofd.ShowDialog();

                if (ofd.ShowDialog() == DialogResult.OK)
                {

                    file_path = ofd.FileName;
                    file_name = ofd.SafeFileName;

                    //string tfile_path = Path.GetDirectoryName(file_path) + "\\";

                }
            }
        }   // end of select_file

    }   // end of class

}   // end of namespace common
