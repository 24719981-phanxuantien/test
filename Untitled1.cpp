
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
/* Bo qua khoang trang */
char *boKhoangTrang(char *chuoi) {
    while (*chuoi == ' ' || *chuoi == '\t' || *chuoi == '\r' || *chuoi == '\n')
        chuoi++;
    return chuoi;
}
 
/* Doc mot so nguyen khong am, kiem tra tran so */
int docSo(char **viTri, long long *giaTri) {
    char *p = boKhoangTrang(*viTri);
    char *cuoi;

    if (*p < '0' || *p > '9')
        return 0;

    *giaTri = strtoll(p, &cuoi, 10);

    if (p == cuoi)
        return 0;

    *viTri = cuoi;
    return 1;
}
 
 /* Doc khoa RSA: so mu va modulo */
int docKhoa(char *chuoiKhoa, long long *soMu, long long *modulo) {
    char *viTri = chuoiKhoa, *sauSoMu;
 
    if (!docSo(&viTri, soMu) || *soMu == 0)
        return 0;
    sauSoMu = viTri;
    viTri = boKhoangTrang(viTri);
    if (*viTri == ',')
        viTri++;
    else if (viTri == sauSoMu)
        return 0;
    if (!docSo(&viTri, modulo) || *modulo <= 255)
        return 0;
    return *boKhoangTrang(viTri) == '\0';
}
 
 /* Cong modulo an toan (a, b < modulo) */
long long congMod(long long a, long long b, long long modulo) {
    if (a >= modulo - b)
        return a - (modulo - b);
    return a + b;
}
 
/* Nhan modulo an toan, khong tran so */
long long nhanMod(long long a, long long b, long long modulo) {
    long long ketQua = 0;
    a %= modulo;
    while (b > 0) {
        if (b & 1)
            ketQua = congMod(ketQua, a, modulo);
        b >>= 1;
        if (b > 0)
            a = congMod(a, a, modulo);
    }
    return ketQua;
}
 
/* Tinh coSo^soMu mod modulo */
long long luyThuaMod(long long coSo, long long soMu, long long modulo) {
    long long ketQua = 1;
    coSo %= modulo;
    while (soMu > 0) {
        if (soMu & 1)
            ketQua = nhanMod(ketQua, coSo, modulo);
        coSo = nhanMod(coSo, coSo, modulo);
        soMu >>= 1;
    }
    return ketQua;
}
 
/* Doc mot dong: 1 = ok, 0 = EOF, -1 = qua dai */
int docDong(char *chuoi, int kichThuoc) {
    if (!fgets(chuoi, kichThuoc, stdin))
        return 0;
 
    if (!strchr(chuoi, '\n')) {
        int c = getchar();
        if (c != EOF && c != '\n') {
            while (c != '\n' && c != EOF)
                c = getchar();
            return -1;
        }
    }
    return 1;
}
 
int main() {
    char dauVao[1000], dongNhap[1000], dongKhoa[1000], banRo[1000];
    long long khoa, modulo, luaChon;
 
    while (1) {
        printf("\n===== RSA =====\n");
        printf("1. Ma hoa\n2. Giai ma\n0. Thoat\nChon: ");
 
        int tt = docDong(dongNhap, sizeof(dongNhap));
        if (tt == 0) break;
        if (tt == -1) {
            printf("Loi: Du lieu qua dai!\n");
            continue;
        }
        char *p = dongNhap;
        if (!docSo(&p, &luaChon) || luaChon > 2 ||
            *boKhoangTrang(p) != '\0') {
            printf("Loi: Lua chon khong hop le!\n");
            continue;
        }
        if (luaChon == 0) break;
        printf("Nhap dau vao: ");
        tt = docDong(dauVao, sizeof(dauVao));
        if (tt <= 0) {
            printf("Loi: Dau vao khong hop le!\n");
            if (tt == 0) break;
            docDong(dongKhoa, sizeof(dongKhoa)); /* bo dong khoa de khong lech dong */
            continue;
        }
        dauVao[strcspn(dauVao, "\r\n")] = '\0';
        printf("Nhap khoa (e,n hoac d,n): ");
        tt = docDong(dongKhoa, sizeof(dongKhoa));
        if (tt <= 0) {
            printf("Loi: Khoa khong hop le!\n");
            if (tt == 0) break;
            continue;
        }
        if (!docKhoa(dongKhoa, &khoa, &modulo)) {
            printf("Loi: Khoa khong hop le!\n");
            continue;
        }
 
        if (luaChon == 1) {                       /* MA HOA */
            if (dauVao[0] == '\0') {
                printf("Loi: Dau vao rong!\n");
                continue;
            }
            printf("Ket qua: ");
            for (int i = 0; dauVao[i] != '\0'; i++)
                printf("%s%lld", i ? " " : "",
                       luyThuaMod((unsigned char)dauVao[i], khoa, modulo));
            printf("\n");
        } else {                                  /* GIAI MA */
            char *q = dauVao;
            int soKyTu = 0, hopLe = 1;
            long long soMaHoa, soGiaiMa;
 
            while (1) {
                q = boKhoangTrang(q);
                if (*q == '\0') break;
 
                if (!docSo(&q, &soMaHoa) || soMaHoa >= modulo ||
                    soKyTu >= 999) {
                    hopLe = 0;
                    break;
                }
 
                soGiaiMa = luyThuaMod(soMaHoa, khoa, modulo);
                if (soGiaiMa > 255) {
                    hopLe = 0;
                    break;
                }
                banRo[soKyTu++] = (char)soGiaiMa;
 
                q = boKhoangTrang(q);
                if (*q == ',') {
                    q = boKhoangTrang(q + 1);
                    if (*q == '\0' || *q == ',') {
                        hopLe = 0;
                        break;
                    }
                } else if (*q != '\0' && (*q < '0' || *q > '9')) {
                    hopLe = 0;
                    break;
                }
            }
 
            if (!hopLe || soKyTu == 0)
                printf("Loi: Day so ma hoa khong hop le!\n");
            else {
                printf("Ket qua: ");
                fwrite(banRo, 1, soKyTu, stdout);
                printf("\n");
            }
        }
    }
    return 0;
}
