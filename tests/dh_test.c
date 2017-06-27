/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */
#include <tomcrypt_test.h>

#ifdef LTC_MDH

#ifdef DH4096
#define KEYSIZE 4096
#else
#define KEYSIZE 2048
#endif

static int _prime_test(void)
{
   void *p, *g, *tmp;
   int x, err, primality;

   if ((err = mp_init_multi(&p, &g, &tmp, NULL)) != CRYPT_OK)               { goto error; }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      if ((err = mp_read_radix(g, ltc_dh_sets[x].base, 16)) != CRYPT_OK)    { goto error; }
      if ((err = mp_read_radix(p, ltc_dh_sets[x].prime, 16)) != CRYPT_OK)   { goto error; }

      /* ensure p is prime */
      if ((err = mp_prime_is_prime(p, 8, &primality)) != CRYPT_OK)          { goto done; }
      if (primality != LTC_MP_YES ) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      if ((err = mp_sub_d(p, 1, tmp)) != CRYPT_OK)                          { goto error; }
      if ((err = mp_div_2(tmp, tmp)) != CRYPT_OK)                           { goto error; }

      /* ensure (p-1)/2 is prime */
      if ((err = mp_prime_is_prime(tmp, 8, &primality)) != CRYPT_OK)        { goto done; }
      if (primality == 0) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }

      /* now see if g^((p-1)/2) mod p is in fact 1 */
      if ((err = mp_exptmod(g, tmp, p, tmp)) != CRYPT_OK)                   { goto error; }
      if (mp_cmp_d(tmp, 1)) {
         err = CRYPT_FAIL_TESTVECTOR;
         goto done;
      }
   }
   err = CRYPT_OK;
error:
done:
   mp_clear_multi(tmp, g, p, NULL);
   return err;
}

static int _dhparam_test(void)
{
   dh_key k;
   unsigned char buf[1024];
   /* generated by: openssl dhparam -outform der -out dhparam.der 2048 */
   unsigned char dhparam_der[] = {
      0x30, 0x82, 0x01, 0x08, 0x02, 0x82, 0x01, 0x01, 0x00, 0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7,
      0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95, 0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0,
      0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f, 0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43,
      0x55, 0x08, 0xb4, 0x20, 0x60, 0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7,
      0x00, 0x44, 0xc2, 0x5e, 0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c,
      0xac, 0x64, 0x13, 0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91,
      0xa4, 0x6e, 0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1,
      0xdb, 0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65, 0x99,
      0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52, 0x4c, 0xd4,
      0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06, 0xd2, 0x0f, 0x2d,
      0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa, 0x67, 0x26, 0xe2, 0x89,
      0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc, 0x70, 0x7e, 0x4f, 0x0e, 0xd3,
      0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b, 0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23,
      0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e, 0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d,
      0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71, 0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86,
      0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1, 0x98, 0xcb, 0x02, 0x01, 0x02
   };
   /* text dump: openssl dh -inform DER -in dhparam.der -text
      DH Parameters: (2048 bit)
          prime:
              00:ae:fe:78:ce:80:d5:d7:8e:cc:4f:0c:1b:b0:95:
              10:e1:41:15:53:4d:0e:68:b0:f8:5a:41:0e:65:2f:
              9f:ac:9c:30:b0:76:ec:02:e9:43:55:08:b4:20:60:
              d9:52:da:2d:ab:9a:ba:e6:cf:11:a7:00:44:c2:5e:
              d1:ba:9b:aa:fe:03:dd:dc:ef:41:89:9c:ac:64:13:
              d9:6a:8a:55:a0:5b:ff:12:92:37:52:6a:91:a4:6e:
              9e:61:b7:fe:b0:17:8e:67:0f:88:46:a7:9e:b1:db:
              68:77:70:b5:77:f2:7e:33:b1:3e:10:c4:63:36:d0:
              13:27:d3:29:c3:b6:5d:f6:5d:a7:d8:25:5c:0b:65:
              99:fa:f9:5f:1d:ee:d1:86:64:7c:44:cb:a0:12:52:
              4c:d4:46:81:ae:07:db:c7:13:29:ce:9b:cf:1c:06:
              d2:0f:2d:bb:12:33:b9:b1:0f:67:5d:3f:0c:e4:fa:
              67:26:e2:89:a2:d5:66:29:1c:e2:8e:bb:7b:cb:cc:
              70:7e:4f:0e:d3:5d:64:64:1b:27:bb:da:a9:08:2b:
              62:d4:ca:c3:3a:23:39:58:57:af:7b:8b:0c:5b:2e:
              fc:42:57:59:39:2e:6d:39:97:db:5b:5c:b9:59:71:
              42:f3:cd:ea:da:86:54:86:61:8d:93:66:c7:65:d1:
              98:cb
          generator: 2 (0x2)
   */
   unsigned char prime[] = {
            0xae, 0xfe, 0x78, 0xce, 0x80, 0xd5, 0xd7, 0x8e, 0xcc, 0x4f, 0x0c, 0x1b, 0xb0, 0x95,
      0x10, 0xe1, 0x41, 0x15, 0x53, 0x4d, 0x0e, 0x68, 0xb0, 0xf8, 0x5a, 0x41, 0x0e, 0x65, 0x2f,
      0x9f, 0xac, 0x9c, 0x30, 0xb0, 0x76, 0xec, 0x02, 0xe9, 0x43, 0x55, 0x08, 0xb4, 0x20, 0x60,
      0xd9, 0x52, 0xda, 0x2d, 0xab, 0x9a, 0xba, 0xe6, 0xcf, 0x11, 0xa7, 0x00, 0x44, 0xc2, 0x5e,
      0xd1, 0xba, 0x9b, 0xaa, 0xfe, 0x03, 0xdd, 0xdc, 0xef, 0x41, 0x89, 0x9c, 0xac, 0x64, 0x13,
      0xd9, 0x6a, 0x8a, 0x55, 0xa0, 0x5b, 0xff, 0x12, 0x92, 0x37, 0x52, 0x6a, 0x91, 0xa4, 0x6e,
      0x9e, 0x61, 0xb7, 0xfe, 0xb0, 0x17, 0x8e, 0x67, 0x0f, 0x88, 0x46, 0xa7, 0x9e, 0xb1, 0xdb,
      0x68, 0x77, 0x70, 0xb5, 0x77, 0xf2, 0x7e, 0x33, 0xb1, 0x3e, 0x10, 0xc4, 0x63, 0x36, 0xd0,
      0x13, 0x27, 0xd3, 0x29, 0xc3, 0xb6, 0x5d, 0xf6, 0x5d, 0xa7, 0xd8, 0x25, 0x5c, 0x0b, 0x65,
      0x99, 0xfa, 0xf9, 0x5f, 0x1d, 0xee, 0xd1, 0x86, 0x64, 0x7c, 0x44, 0xcb, 0xa0, 0x12, 0x52,
      0x4c, 0xd4, 0x46, 0x81, 0xae, 0x07, 0xdb, 0xc7, 0x13, 0x29, 0xce, 0x9b, 0xcf, 0x1c, 0x06,
      0xd2, 0x0f, 0x2d, 0xbb, 0x12, 0x33, 0xb9, 0xb1, 0x0f, 0x67, 0x5d, 0x3f, 0x0c, 0xe4, 0xfa,
      0x67, 0x26, 0xe2, 0x89, 0xa2, 0xd5, 0x66, 0x29, 0x1c, 0xe2, 0x8e, 0xbb, 0x7b, 0xcb, 0xcc,
      0x70, 0x7e, 0x4f, 0x0e, 0xd3, 0x5d, 0x64, 0x64, 0x1b, 0x27, 0xbb, 0xda, 0xa9, 0x08, 0x2b,
      0x62, 0xd4, 0xca, 0xc3, 0x3a, 0x23, 0x39, 0x58, 0x57, 0xaf, 0x7b, 0x8b, 0x0c, 0x5b, 0x2e,
      0xfc, 0x42, 0x57, 0x59, 0x39, 0x2e, 0x6d, 0x39, 0x97, 0xdb, 0x5b, 0x5c, 0xb9, 0x59, 0x71,
      0x42, 0xf3, 0xcd, 0xea, 0xda, 0x86, 0x54, 0x86, 0x61, 0x8d, 0x93, 0x66, 0xc7, 0x65, 0xd1,
      0x98, 0xcb
   };

   DO(dh_make_key_dhparam(&yarrow_prng, find_prng ("yarrow"), dhparam_der, sizeof(dhparam_der), &k));
   if (mp_unsigned_bin_size(k.prime) > sizeof(buf)) {
      printf("dhparam_test: short buf\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   DO(mp_to_unsigned_bin(k.prime, buf));
   if (compare_testvector(buf, sizeof(prime), prime, sizeof(prime), "dhparam_test", 1)) {
      printf("dhparam_test: prime mismatch\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   if (mp_cmp_d(k.base, 2) != LTC_MP_EQ) {
      printf("dhparam_test: base mismatch\n");
      dh_free(&k);
      return CRYPT_ERROR;
   }
   dh_free(&k);
   return CRYPT_OK;
}

static int _radix_test(void)
{
   dh_key k1 = LTC_DH_KEY_INITIALIZER;
   dh_key k2 = LTC_DH_KEY_INITIALIZER;
   dh_key k3 = LTC_DH_KEY_INITIALIZER;
   unsigned char buf[4096];
   unsigned long len;
   int i, j;
   /* RADIX 16 */
   char *ghex = "2";
   char *phex = "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22"
                "514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6"
                "F44C42E9A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D"
                "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB"
                "9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C180E8603"
                "9B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF6955817183995497CEA956AE515D2261898FA0510"
                "15728E5A8AACAA68FFFFFFFFFFFFFFFF";
   char *xhex = "A6681ADC386CE944C3DED9A7301DCC9C518250E3EDB62F959198F8DC0057DD6FB57ABAFD788198B1";
   char *yhex = "39046632C834418DFA07B3091538B614D1FB5DBB785C0FBEA3B98B295BC0CD076A88D9452141A269"
                "E8BAEB1DD654EBA03A5705318D129754CDF4003A8C399240FBB8F162490F6F0DC70E414B6FEE8808"
                "6AFAA48E9F3A248EDC093452663D34E0E809D4F6BADBB36F80B6813EBF7C3281B862209E5604BDEA"
                "8B8F5F7BFDC3EEB7ADB73048289BCEA0F5A5CDEE7DF91CD1F0BA632F06DBE9BA7EF014B84B02D497"
                "CA7D0C60F734752A649DA496946B4E531B30D9F82EDD855636C0B0F2AE232E4186454E8887BB423E"
                "32A5A2495EACBA99620ACD03A38345EBB6735E62330A8EE9AA6C8370410F5CD45AF37EE90A0DA95B"
                "E96FC939E88FE0BD2CD09FC8F524208C";
   /* RADIX 47 */
   char *gr47 = "2";
   char *pr47 = "F27Mg1SadOFIRbDOJ5dHgHiVF02Z1LHHQ6G5SLG2U8aTdfH1ETk4GARRE7WW99dBUBLb9e2OHFIaSM1A"
                "ag2LNNjgYa9I9CjQGJihL3J7A2SGQe8j5Ch8EHMj5jVbAYDiQKhhPhM6Hc56fKS40GUfJkGO7KJ6EXZQ"
                "VgbSa2AkPC65F91g0PaYie8AGNVaFKaV9HOQf3ia1iW4i6eCOB9CcBbH7TbQij8AEgjZ0VRBcLKc6UYO"
                "1Zc3I2Jc0h1H2HBEH8ONI3OYBbaPV6XhAd8WCc60D0RDBU3H9U7cWL28a0c90XNO0dh5RXEFBbUCE2ZG"
                "gh9XQSVIHkVbFIS5F5IGVOkiWAVc9i8BHB2V0UbGW6UdRTZVV";
   char *xr47 = "6bhO7O9NWFRgEMjdU0Y5POj3c1JP15MYEdIg3FO1PEjUY2aGYNSXcaF01R";
   char *yr47 = "3GNPNWEYfKML1cIbI7Cc1Z0O7aQLJgB734dO2i56LLYDdI4gHYk2GAbQH2WI97hNeC7dj3fPEH8I9gV9"
                "U323AXj1AJXbFPFIHGOTdC29QUUeH2SSc6NWhfQDDXd5Q5iXCKEAUGX3SKcNFIfVOYJgZCLjfHYQdgOQ"
                "GCjKNgbEV7Hj34MU3b79iANX2DbMYfb9iGi78BWH2HYAd7IAhk7U0OYGHKJX1bIUUj1KBLhAUg46GaER"
                "G9W3ARMfBCj6kSdDF9TdkWAjWTDj722IeVJERC4bKU2VDFG20kDhCMF985efD1SS8DfXcdCHF1kDUkSA"
                "884FHYiFEPkaagQOBQaN9BNaEHNbbd002DCIIX5eMP4HgPJPF";
   /* RADIX 64 */
   char *gr64 = "2";
   char *pr64 = "3//////////yaFsg8XQC8qnCPYYu3S7D4f0au8YcVCT08BlgOx4viYKKe8UOuq1DtlbHcppJf36p0h2c"
                "toNnGtJ+4rRMrHmaNaXRLsObv+nlHCGkccD+rh2/zSjlG6j+tkE6lxMecVfQwV915yIn/cIIXcKUpaMp"
                "t207oueME/1PZQI3OSLTEQQHO/gFqapr+3PLqZtAEjbXnYyrOWXLAxdjKf1t2Mbcrd33LEIhoO1F5qR0"
                "ZA625yCf1UHYuspZlZddSi60w60vidWwBi1wAFjSLTy6zCKidUAylsbLWN63cLINpgbMhb5T8c69Zw1H"
                "0LSevQYgogQF//////////";
   char *xr64 = "2cQ1hSE6pfHCFUsQSm7SoSKO9Gu+ssBvMHcFZS05VTRxLwklruWPYn";
   char *yr64 = "v16Ooo3H1ZVe7imaLEBOKqVjTktXS3xwZkOifMy3D1sg8sKKXGQ9fwBhh7TPKww0wLmKnZHANLCtq03g"
                "CEP90+xZnOaaFRmt73a5BR+w826hwf8wVEYIEt0aqKcOzDE3e2TJskjkpRu2sWJw/V3A1k68WdbO4lUg"
                "BZrzx/SFkjwstC4WecywWzQNDxdtv7D7mkcCl1jlfkdxm5BXB0jINodqCOFSqTIfadQIMb6jEKnimsVW"
                "ktOLMDi2myguZBa66HKw8Xxj2FZAbeabUhBgPOWhD0wE3HUksSrvYCmgEwQfiWt113rpKMlD+wGeDgLl"
                "fRyavw8/WlIpGdyZr922C";
   /* RADIX 256 */
   unsigned char gbin[] = { 0x02 };
   unsigned char pbin[] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
   };
   unsigned char xbin[] = {
      0xA6, 0x68, 0x1A, 0xDC, 0x38, 0x6C, 0xE9, 0x44, 0xC3, 0xDE, 0xD9, 0xA7, 0x30, 0x1D, 0xCC, 0x9C,
      0x51, 0x82, 0x50, 0xE3, 0xED, 0xB6, 0x2F, 0x95, 0x91, 0x98, 0xF8, 0xDC, 0x00, 0x57, 0xDD, 0x6F,
      0xB5, 0x7A, 0xBA, 0xFD, 0x78, 0x81, 0x98, 0xB1
   };
   unsigned char ybin[] = {
      0x39, 0x04, 0x66, 0x32, 0xC8, 0x34, 0x41, 0x8D, 0xFA, 0x07, 0xB3, 0x09, 0x15, 0x38, 0xB6, 0x14,
      0xD1, 0xFB, 0x5D, 0xBB, 0x78, 0x5C, 0x0F, 0xBE, 0xA3, 0xB9, 0x8B, 0x29, 0x5B, 0xC0, 0xCD, 0x07,
      0x6A, 0x88, 0xD9, 0x45, 0x21, 0x41, 0xA2, 0x69, 0xE8, 0xBA, 0xEB, 0x1D, 0xD6, 0x54, 0xEB, 0xA0,
      0x3A, 0x57, 0x05, 0x31, 0x8D, 0x12, 0x97, 0x54, 0xCD, 0xF4, 0x00, 0x3A, 0x8C, 0x39, 0x92, 0x40,
      0xFB, 0xB8, 0xF1, 0x62, 0x49, 0x0F, 0x6F, 0x0D, 0xC7, 0x0E, 0x41, 0x4B, 0x6F, 0xEE, 0x88, 0x08,
      0x6A, 0xFA, 0xA4, 0x8E, 0x9F, 0x3A, 0x24, 0x8E, 0xDC, 0x09, 0x34, 0x52, 0x66, 0x3D, 0x34, 0xE0,
      0xE8, 0x09, 0xD4, 0xF6, 0xBA, 0xDB, 0xB3, 0x6F, 0x80, 0xB6, 0x81, 0x3E, 0xBF, 0x7C, 0x32, 0x81,
      0xB8, 0x62, 0x20, 0x9E, 0x56, 0x04, 0xBD, 0xEA, 0x8B, 0x8F, 0x5F, 0x7B, 0xFD, 0xC3, 0xEE, 0xB7,
      0xAD, 0xB7, 0x30, 0x48, 0x28, 0x9B, 0xCE, 0xA0, 0xF5, 0xA5, 0xCD, 0xEE, 0x7D, 0xF9, 0x1C, 0xD1,
      0xF0, 0xBA, 0x63, 0x2F, 0x06, 0xDB, 0xE9, 0xBA, 0x7E, 0xF0, 0x14, 0xB8, 0x4B, 0x02, 0xD4, 0x97,
      0xCA, 0x7D, 0x0C, 0x60, 0xF7, 0x34, 0x75, 0x2A, 0x64, 0x9D, 0xA4, 0x96, 0x94, 0x6B, 0x4E, 0x53,
      0x1B, 0x30, 0xD9, 0xF8, 0x2E, 0xDD, 0x85, 0x56, 0x36, 0xC0, 0xB0, 0xF2, 0xAE, 0x23, 0x2E, 0x41,
      0x86, 0x45, 0x4E, 0x88, 0x87, 0xBB, 0x42, 0x3E, 0x32, 0xA5, 0xA2, 0x49, 0x5E, 0xAC, 0xBA, 0x99,
      0x62, 0x0A, 0xCD, 0x03, 0xA3, 0x83, 0x45, 0xEB, 0xB6, 0x73, 0x5E, 0x62, 0x33, 0x0A, 0x8E, 0xE9,
      0xAA, 0x6C, 0x83, 0x70, 0x41, 0x0F, 0x5C, 0xD4, 0x5A, 0xF3, 0x7E, 0xE9, 0x0A, 0x0D, 0xA9, 0x5B,
      0xE9, 0x6F, 0xC9, 0x39, 0xE8, 0x8F, 0xE0, 0xBD, 0x2C, 0xD0, 0x9F, 0xC8, 0xF5, 0x24, 0x20, 0x8C
   };

   struct {
     int radix;
     void* g; int glen;
     void* p; int plen;
     void* x; int xlen;
     void* y; int ylen;
   } test[4] = {
      { 16,  ghex, strlen(ghex)+1, phex, strlen(phex)+1, xhex, strlen(xhex)+1, yhex, strlen(yhex)+1 },
      { 47,  gr47, strlen(gr47)+1, pr47, strlen(pr47)+1, xr47, strlen(xr47)+1, yr47, strlen(yr47)+1 },
      { 64,  gr64, strlen(gr64)+1, pr64, strlen(pr64)+1, xr64, strlen(xr64)+1, yr64, strlen(yr64)+1 },
      { 256, gbin, sizeof(gbin),   pbin, sizeof(pbin),   xbin, sizeof(xbin),   ybin, sizeof(ybin)   }
   };

   unsigned char export_private[] = {
      0x30, 0x82, 0x01, 0x3A, 0x02, 0x01, 0x00, 0x03, 0x02, 0x07, 0x80, 0x02, 0x82, 0x01, 0x01, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x02, 0x01, 0x02, 0x02, 0x29, 0x00, 0xA6, 0x68, 0x1A, 0xDC, 0x38, 0x6C, 0xE9, 0x44, 0xC3, 0xDE,
      0xD9, 0xA7, 0x30, 0x1D, 0xCC, 0x9C, 0x51, 0x82, 0x50, 0xE3, 0xED, 0xB6, 0x2F, 0x95, 0x91, 0x98,
      0xF8, 0xDC, 0x00, 0x57, 0xDD, 0x6F, 0xB5, 0x7A, 0xBA, 0xFD, 0x78, 0x81, 0x98, 0xB1
   };
   unsigned char export_public[] = {
      0x30, 0x82, 0x02, 0x13, 0x02, 0x01, 0x00, 0x03, 0x02, 0x07, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC9, 0x0F, 0xDA, 0xA2, 0x21, 0x68, 0xC2, 0x34,
      0xC4, 0xC6, 0x62, 0x8B, 0x80, 0xDC, 0x1C, 0xD1, 0x29, 0x02, 0x4E, 0x08, 0x8A, 0x67, 0xCC, 0x74,
      0x02, 0x0B, 0xBE, 0xA6, 0x3B, 0x13, 0x9B, 0x22, 0x51, 0x4A, 0x08, 0x79, 0x8E, 0x34, 0x04, 0xDD,
      0xEF, 0x95, 0x19, 0xB3, 0xCD, 0x3A, 0x43, 0x1B, 0x30, 0x2B, 0x0A, 0x6D, 0xF2, 0x5F, 0x14, 0x37,
      0x4F, 0xE1, 0x35, 0x6D, 0x6D, 0x51, 0xC2, 0x45, 0xE4, 0x85, 0xB5, 0x76, 0x62, 0x5E, 0x7E, 0xC6,
      0xF4, 0x4C, 0x42, 0xE9, 0xA6, 0x37, 0xED, 0x6B, 0x0B, 0xFF, 0x5C, 0xB6, 0xF4, 0x06, 0xB7, 0xED,
      0xEE, 0x38, 0x6B, 0xFB, 0x5A, 0x89, 0x9F, 0xA5, 0xAE, 0x9F, 0x24, 0x11, 0x7C, 0x4B, 0x1F, 0xE6,
      0x49, 0x28, 0x66, 0x51, 0xEC, 0xE4, 0x5B, 0x3D, 0xC2, 0x00, 0x7C, 0xB8, 0xA1, 0x63, 0xBF, 0x05,
      0x98, 0xDA, 0x48, 0x36, 0x1C, 0x55, 0xD3, 0x9A, 0x69, 0x16, 0x3F, 0xA8, 0xFD, 0x24, 0xCF, 0x5F,
      0x83, 0x65, 0x5D, 0x23, 0xDC, 0xA3, 0xAD, 0x96, 0x1C, 0x62, 0xF3, 0x56, 0x20, 0x85, 0x52, 0xBB,
      0x9E, 0xD5, 0x29, 0x07, 0x70, 0x96, 0x96, 0x6D, 0x67, 0x0C, 0x35, 0x4E, 0x4A, 0xBC, 0x98, 0x04,
      0xF1, 0x74, 0x6C, 0x08, 0xCA, 0x18, 0x21, 0x7C, 0x32, 0x90, 0x5E, 0x46, 0x2E, 0x36, 0xCE, 0x3B,
      0xE3, 0x9E, 0x77, 0x2C, 0x18, 0x0E, 0x86, 0x03, 0x9B, 0x27, 0x83, 0xA2, 0xEC, 0x07, 0xA2, 0x8F,
      0xB5, 0xC5, 0x5D, 0xF0, 0x6F, 0x4C, 0x52, 0xC9, 0xDE, 0x2B, 0xCB, 0xF6, 0x95, 0x58, 0x17, 0x18,
      0x39, 0x95, 0x49, 0x7C, 0xEA, 0x95, 0x6A, 0xE5, 0x15, 0xD2, 0x26, 0x18, 0x98, 0xFA, 0x05, 0x10,
      0x15, 0x72, 0x8E, 0x5A, 0x8A, 0xAC, 0xAA, 0x68, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x02, 0x01, 0x02, 0x02, 0x82, 0x01, 0x00, 0x39, 0x04, 0x66, 0x32, 0xC8, 0x34, 0x41, 0x8D, 0xFA,
      0x07, 0xB3, 0x09, 0x15, 0x38, 0xB6, 0x14, 0xD1, 0xFB, 0x5D, 0xBB, 0x78, 0x5C, 0x0F, 0xBE, 0xA3,
      0xB9, 0x8B, 0x29, 0x5B, 0xC0, 0xCD, 0x07, 0x6A, 0x88, 0xD9, 0x45, 0x21, 0x41, 0xA2, 0x69, 0xE8,
      0xBA, 0xEB, 0x1D, 0xD6, 0x54, 0xEB, 0xA0, 0x3A, 0x57, 0x05, 0x31, 0x8D, 0x12, 0x97, 0x54, 0xCD,
      0xF4, 0x00, 0x3A, 0x8C, 0x39, 0x92, 0x40, 0xFB, 0xB8, 0xF1, 0x62, 0x49, 0x0F, 0x6F, 0x0D, 0xC7,
      0x0E, 0x41, 0x4B, 0x6F, 0xEE, 0x88, 0x08, 0x6A, 0xFA, 0xA4, 0x8E, 0x9F, 0x3A, 0x24, 0x8E, 0xDC,
      0x09, 0x34, 0x52, 0x66, 0x3D, 0x34, 0xE0, 0xE8, 0x09, 0xD4, 0xF6, 0xBA, 0xDB, 0xB3, 0x6F, 0x80,
      0xB6, 0x81, 0x3E, 0xBF, 0x7C, 0x32, 0x81, 0xB8, 0x62, 0x20, 0x9E, 0x56, 0x04, 0xBD, 0xEA, 0x8B,
      0x8F, 0x5F, 0x7B, 0xFD, 0xC3, 0xEE, 0xB7, 0xAD, 0xB7, 0x30, 0x48, 0x28, 0x9B, 0xCE, 0xA0, 0xF5,
      0xA5, 0xCD, 0xEE, 0x7D, 0xF9, 0x1C, 0xD1, 0xF0, 0xBA, 0x63, 0x2F, 0x06, 0xDB, 0xE9, 0xBA, 0x7E,
      0xF0, 0x14, 0xB8, 0x4B, 0x02, 0xD4, 0x97, 0xCA, 0x7D, 0x0C, 0x60, 0xF7, 0x34, 0x75, 0x2A, 0x64,
      0x9D, 0xA4, 0x96, 0x94, 0x6B, 0x4E, 0x53, 0x1B, 0x30, 0xD9, 0xF8, 0x2E, 0xDD, 0x85, 0x56, 0x36,
      0xC0, 0xB0, 0xF2, 0xAE, 0x23, 0x2E, 0x41, 0x86, 0x45, 0x4E, 0x88, 0x87, 0xBB, 0x42, 0x3E, 0x32,
      0xA5, 0xA2, 0x49, 0x5E, 0xAC, 0xBA, 0x99, 0x62, 0x0A, 0xCD, 0x03, 0xA3, 0x83, 0x45, 0xEB, 0xB6,
      0x73, 0x5E, 0x62, 0x33, 0x0A, 0x8E, 0xE9, 0xAA, 0x6C, 0x83, 0x70, 0x41, 0x0F, 0x5C, 0xD4, 0x5A,
      0xF3, 0x7E, 0xE9, 0x0A, 0x0D, 0xA9, 0x5B, 0xE9, 0x6F, 0xC9, 0x39, 0xE8, 0x8F, 0xE0, 0xBD, 0x2C,
      0xD0, 0x9F, 0xC8, 0xF5, 0x24, 0x20, 0x8C
   };
   unsigned char key_parts[4][512];
   unsigned long key_lens[4];

   if (register_prng(&yarrow_desc) == -1) {
      printf("Error registering yarrow PRNG\n");
      return CRYPT_ERROR;
   }

   for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; ++j) {
         key_lens[j] = sizeof(key_parts[j]);
      }
      if(test[i].radix != 256) {
         DO(radix_to_bin(test[i].x, test[i].radix, key_parts[0], &key_lens[0]));
         DO(radix_to_bin(test[i].y, test[i].radix, key_parts[1], &key_lens[1]));
         DO(radix_to_bin(test[i].p, test[i].radix, key_parts[2], &key_lens[2]));
         DO(radix_to_bin(test[i].g, test[i].radix, key_parts[3], &key_lens[3]));

         DO(dh_set_pg(key_parts[2], key_lens[2], key_parts[3], key_lens[3], &k1));
         DO(dh_set_key(NULL, 0, key_parts[0], key_lens[0], &k1));
      }
      else {
         DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k1));
         DO(dh_set_key(NULL, 0, test[i].x, test[i].xlen, &k1));
      }

      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PRIVATE, &k1));
      if (compare_testvector(buf, len, export_private, sizeof(export_private), "radix_test", i*10 + 0)) {
         printf("radix_test: dh_export+PK_PRIVATE mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PUBLIC, &k1));
      if (compare_testvector(buf, len, export_public, sizeof(export_public), "radix_test", i*10 + 1)) {
         printf("radix_test: dh_export+PK_PUBLIC mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export_radix(256, buf, &len, PK_PRIVATE, &k1));
      if (compare_testvector(buf, len, xbin, sizeof(xbin), "radix_test", i*10 + 2)) {
         printf("radix_test: dh_export+PK_PRIVATE mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export_radix(256, buf, &len, PK_PUBLIC, &k1));
      if (compare_testvector(buf, len, ybin, sizeof(ybin), "radix_test", i*10 + 3)) {
         printf("radix_test: dh_export+PK_PUBLIC mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export_radix(47, buf, &len, PK_PRIVATE, &k1));
      if (compare_testvector(buf, len, xr47, strlen(xr47)+1, "radix_test", i*10 + 4)) {
         printf("radix_test: dh_export+PK_PRIVATE mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export_radix(47, buf, &len, PK_PUBLIC, &k1));
      if (compare_testvector(buf, len, yr47, strlen(yr47)+1, "radix_test", i*10 + 5)) {
         printf("radix_test: dh_export+PK_PUBLIC mismatch\n");
         dh_free(&k1);
         return CRYPT_ERROR;
      }
      dh_free(&k1);

      if(test[i].radix != 256) {
         DO(dh_set_pg(key_parts[2], key_lens[2], key_parts[3], key_lens[3], &k2));
         DO(dh_set_key(key_parts[1], key_lens[1], NULL, 0, &k2));
      }
      else {
         DO(dh_set_pg(test[i].p, test[i].plen, test[i].g, test[i].glen, &k2));
         DO(dh_set_key(test[i].y, test[i].ylen, NULL, 0, &k2));
      }

      len = sizeof(buf);
      DO(dh_export(buf, &len, PK_PUBLIC, &k2));
      if (compare_testvector(buf, len, export_public, sizeof(export_public), "radix_test", i*10 + 6)) {
         printf("radix_test: dh_export+PK_PUBLIC mismatch\n");
         dh_free(&k2);
         return CRYPT_ERROR;
      }
      len = sizeof(buf);
      DO(dh_export_radix(256, buf, &len, PK_PUBLIC, &k2));
      if (compare_testvector(buf, len, ybin, sizeof(ybin), "radix_test", i*10 + 7)) {
         printf("radix_test: dh_export+PK_PUBLIC mismatch\n");
         dh_free(&k2);
         return CRYPT_ERROR;
      }
      dh_free(&k2);

      DO(dh_make_key_ex(&yarrow_prng, find_prng("yarrow"), test[i].radix,
                        test[i].p, test[i].plen, test[i].g, test[i].glen, &k3));
      len = mp_unsigned_bin_size(k3.prime);
      DO(mp_to_unsigned_bin(k3.prime, buf));
      if (compare_testvector(buf, len, pbin, sizeof(pbin), "radix_test", i*10 + 8)) {
         printf("radix_test: dh_make_key_ex prime mismatch\n");
         dh_free(&k3);
         return CRYPT_ERROR;
      }
      len = mp_unsigned_bin_size(k3.base);
      DO(mp_to_unsigned_bin(k3.base, buf));
      if (compare_testvector(buf, len, gbin, sizeof(gbin), "radix_test", i*10 + 9)) {
         printf("radix_test: dh_make_key_ex base mismatch\n");
         dh_free(&k3);
         return CRYPT_ERROR;
      }
      dh_free(&k3);
   }

   return CRYPT_OK;
}

static int _basic_test(void)
{
   unsigned char buf[3][4096];
   unsigned long x, y, z;
   int           size;
   dh_key        usera, userb;

   if (register_prng(&yarrow_desc) == -1) {
      printf("Error registering yarrow PRNG\n");
      return CRYPT_ERROR;
   }
   if (register_hash(&md5_desc) == -1) {
      printf("Error registering md5 hash\n");
      return CRYPT_ERROR;
   }

   /* make up two keys */
   DO(dh_make_key (&yarrow_prng, find_prng ("yarrow"), KEYSIZE/8, &usera));
   DO(dh_make_key (&yarrow_prng, find_prng ("yarrow"), KEYSIZE/8, &userb));

   /* make the shared secret */
   x = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[0], &x));

   y = KEYSIZE;
   DO(dh_shared_secret (&userb, &usera, buf[1], &y));
   if (y != x) {
      fprintf(stderr, "DH Shared keys are not same size.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[1], x)) {
      fprintf(stderr, "DH Shared keys not same contents.\n");
      dh_free (&usera);
      dh_free (&userb);
      return CRYPT_ERROR;
   }

   /* now export userb */
   y = KEYSIZE;
   DO(dh_export (buf[1], &y, PK_PUBLIC, &userb));
   dh_free (&userb);

   /* import and make the shared secret again */
   DO(dh_import (buf[1], y, &userb));
   z = KEYSIZE;
   DO(dh_shared_secret (&usera, &userb, buf[2], &z));

   dh_free (&usera);
   dh_free (&userb);

   if (z != x) {
      fprintf(stderr, "failed.  Size don't match?\n");
      return CRYPT_ERROR;
   }
   if (memcmp (buf[0], buf[2], x)) {
      fprintf(stderr, "Failed.  Content didn't match.\n");
      return CRYPT_ERROR;
   }

   for (x = 0; ltc_dh_sets[x].size != 0; x++) {
      DO(dh_make_key(&yarrow_prng, find_prng ("yarrow"), ltc_dh_sets[x].size, &usera));
      size = dh_get_groupsize(&usera);
      dh_free(&usera);
      if (size != ltc_dh_sets[x].size) {
         fprintf(stderr, "dh_groupsize mismatch %d %d\n", size, ltc_dh_sets[x].size);
         return CRYPT_ERROR;
      }
      DO(dh_make_key_ex(&yarrow_prng, find_prng ("yarrow"), 16,
                        ltc_dh_sets[x].prime, strlen(ltc_dh_sets[x].prime) + 1,
                        ltc_dh_sets[x].base,  strlen(ltc_dh_sets[x].base)  + 1,
                        &usera));
      dh_free(&usera);
   }

   return CRYPT_OK;
}

int dh_test(void)
{
   int fails = 0;
   if (_prime_test() != CRYPT_OK) fails++;
   if (_basic_test() != CRYPT_OK) fails++;
   if (_dhparam_test() != CRYPT_OK) fails++;
   if (_radix_test() != CRYPT_OK) fails++;
   return fails > 0 ? CRYPT_FAIL_TESTVECTOR : CRYPT_OK;
}

#else

int dh_test(void)
{
   return CRYPT_NOP;
}

#endif

/* ref:         $Format:%D$ */
/* git commit:  $Format:%H$ */
/* commit time: $Format:%ai$ */
