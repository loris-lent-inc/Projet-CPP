using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Imaging;

namespace libImage
{
    public class ClImage
    {
        // on crée une classe C# avec pointeur sur l'objet C++
        // puis des static extern exportées de chaque méthode utile de la classe C++
        public IntPtr ClPtr;
        public double tempsTraitement;
        public Image source = null;
        //public Image gtruth = null;
        public Image result = null;

        public ClImage()
        {
            ClPtr = IntPtr.Zero;
        }

        ~ClImage()
        {
            if (ClPtr != IntPtr.Zero)
                ClPtr = IntPtr.Zero;
        }


        // va-et-vient avec constructeur C#/C++
        // obligatoire dans toute nouvelle classe propre à l'application

        [DllImport("libImage.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr objetLib();

        public IntPtr objetLibPtr()
        {
            ClPtr = objetLib();
            return ClPtr;
        }

        [DllImport("libImage.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr objetLibDataImg(int nbChamps, IntPtr data, IntPtr refIm, int stride, int nbLig, int nbCol);

        public IntPtr objetLibDataImgPtr(int nbChamps, IntPtr data, IntPtr refIm, int stride, int nbLig, int nbCol)
        {
            ClPtr = objetLibDataImg(nbChamps,data, refIm, stride, nbLig, nbCol);
            return ClPtr;
        }

        [DllImport("libImage.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double valeurChamp(IntPtr pImg, int i);

        public double objetLibValeurChamp(int i)
        {
            return valeurChamp(ClPtr, i);
        }

        public static ClImage traiter(Tuple<Image, Image> tuple)
        {
            ClImage Img = new ClImage();
            
            Img.source = tuple.Item1;
            //Img.gtruth = tuple.Item2;

            Bitmap sourceBMP = new Bitmap(Img.source);
            Bitmap GTBMP = new Bitmap(tuple.Item2);

            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
            unsafe
            {
                BitmapData sourceBMPData = sourceBMP.LockBits(new Rectangle(0, 0, sourceBMP.Width, sourceBMP.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                BitmapData GTbmpData = GTBMP.LockBits(new Rectangle(0, 0, GTBMP.Width, GTBMP.Height), ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);
                Img.objetLibDataImgPtr(2, sourceBMPData.Scan0, GTbmpData.Scan0, sourceBMPData.Stride, sourceBMP.Height, sourceBMP.Width);
                // 1 champ texte retour C++, le seuil auto
                sourceBMP.UnlockBits(sourceBMPData);
                GTBMP.UnlockBits(GTbmpData);
            }
            stopwatch.Stop();
            TimeSpan elapsedTime = stopwatch.Elapsed;
            
            Img.tempsTraitement = elapsedTime.TotalSeconds;
            Img.result = new Bitmap(sourceBMP);

            return Img;
        }
    }
}
