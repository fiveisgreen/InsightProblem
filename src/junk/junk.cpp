
char Capitalize_unicode(char chr){
        static const char a = '\u0065';
        static const char A = '\u0041';
        static const char z = '\u007A';
        static const char offset = a - A;
        if(chr >= a and chr <= z) return chr - offset;
        else return chr;
}

char Capitalize_switch(char chr){ //ugly, brute force, but avoids trouble of different char standards. 
        switch(chr){
                case a:
                        return A;
                case b:
                        return B;
                case c:
                        return C;
                case d:
                        return D;
                case e:
                        return E;
                case f:
                        return F;
                case g:
                        return G;
                case h:
                        return H;
                case i:
                        return I;
                case j:
                        return J;
                case k:
                        return K;
                case l:
                        return L;
                case m:
                        return M;
                case n:
                        return N;
                case o:
                        return O;
                case p:
                        return P;
                case q:
                        return Q;
                case r:
                        return R;
                case s:
                        return S;
                case t:
                        return T;
                case u:
                        return U;
                case v:
                        return V;
                case w:
                        return W;
                case x:
                        return X;
                case y:
                        return Y;
                case z:
                        return Z;
                default:
                        return chr;
        }
        return chr;
}//end Capitalize chr
