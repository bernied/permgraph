BEG_G {
   node_t n;
   int i, cnt = $.n_nodes;
   double pi = atan2(0,-1);
   double maxsz = 72;     // maxinum node diameter in points
   double r = cnt*(1 + maxsz)/(2*pi);   // radius of layout circle
   double theta = 2*pi/cnt;

   i = 0;
   for (n = fstnode($);n;n = nxtnode(n)) {
//     n.pos = sprintf ("%f,%f", r*cos(i*theta), r*sin(i*theta));
     n.pos = sprintf ("%f,%f", r*cos(-i*theta+pi/2), r*sin(-i*theta+pi/2));
     i++;
   }
}

