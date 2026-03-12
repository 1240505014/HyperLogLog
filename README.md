# HyperLogLog (HLL) Cardinality Estimation

Bu proje, Büyük Veri Analitiğinde kullanılan olasılıksal veri yapılarından biri olan HyperLogLog (HLL) algoritmasının C dili kullanılarak sıfırdan tasarlanmış bir gerçeklemesidir.

## Özellikler
- **Yüksek Kaliteli Hashing:** FNV-1a 32-bit hash fonksiyonu kullanılmıştır.
- **Bucketing & Register:** Girdilerin ilk p biti kova indeksi, kalan bitler `__builtin_clz` ile ardışık sıfır sayımı (rho) için kullanılmıştır.
- **Harmonik Ortalama & Düzeltmeler:** Küçük veri setleri (Linear Counting) ve büyük veri setleri için sınır düzeltmeleri entegre edilmiştir.
- **Birleştirilebilirlik (Mergeable):** İki farklı HLL yapısı (`hll_merge`) veri kaybı olmadan birleştirilebilir.

## Kurulum ve Çalıştırma
GCC derleyicisi (Dev-C++) ile derlenip çalıştırılabilir.
