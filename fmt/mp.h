7100 // See MultiProcessor Specification Version 1.[14]
7101 
7102 struct mp {             // floating pointer
7103   uchar signature[4];           // "_MP_"
7104   void *physaddr;               // phys addr of MP config table
7105   uchar length;                 // 1
7106   uchar specrev;                // [14]
7107   uchar checksum;               // all bytes must add up to 0
7108   uchar type;                   // MP system config type
7109   uchar imcrp;
7110   uchar reserved[3];
7111 };
7112 
7113 struct mpconf {         // configuration table header
7114   uchar signature[4];           // "PCMP"
7115   ushort length;                // total table length
7116   uchar version;                // [14]
7117   uchar checksum;               // all bytes must add up to 0
7118   uchar product[20];            // product id
7119   uint *oemtable;               // OEM table pointer
7120   ushort oemlength;             // OEM table length
7121   ushort entry;                 // entry count
7122   uint *lapicaddr;              // address of local APIC
7123   ushort xlength;               // extended table length
7124   uchar xchecksum;              // extended table checksum
7125   uchar reserved;
7126 };
7127 
7128 struct mpproc {         // processor table entry
7129   uchar type;                   // entry type (0)
7130   uchar apicid;                 // local APIC id
7131   uchar version;                // local APIC verison
7132   uchar flags;                  // CPU flags
7133     #define MPBOOT 0x02           // This proc is the bootstrap processor.
7134   uchar signature[4];           // CPU signature
7135   uint feature;                 // feature flags from CPUID instruction
7136   uchar reserved[8];
7137 };
7138 
7139 struct mpioapic {       // I/O APIC table entry
7140   uchar type;                   // entry type (2)
7141   uchar apicno;                 // I/O APIC id
7142   uchar version;                // I/O APIC version
7143   uchar flags;                  // I/O APIC flags
7144   uint *addr;                  // I/O APIC address
7145 };
7146 
7147 
7148 
7149 
7150 // Table entry types
7151 #define MPPROC    0x00  // One per processor
7152 #define MPBUS     0x01  // One per bus
7153 #define MPIOAPIC  0x02  // One per I/O APIC
7154 #define MPIOINTR  0x03  // One per bus interrupt source
7155 #define MPLINTR   0x04  // One per system interrupt source
7156 
7157 
7158 
7159 
7160 
7161 
7162 
7163 
7164 
7165 
7166 
7167 
7168 
7169 
7170 
7171 
7172 
7173 
7174 
7175 
7176 
7177 
7178 
7179 
7180 
7181 
7182 
7183 
7184 
7185 
7186 
7187 
7188 
7189 
7190 
7191 
7192 
7193 
7194 
7195 
7196 
7197 
7198 
7199 
7200 // Blank page.
7201 
7202 
7203 
7204 
7205 
7206 
7207 
7208 
7209 
7210 
7211 
7212 
7213 
7214 
7215 
7216 
7217 
7218 
7219 
7220 
7221 
7222 
7223 
7224 
7225 
7226 
7227 
7228 
7229 
7230 
7231 
7232 
7233 
7234 
7235 
7236 
7237 
7238 
7239 
7240 
7241 
7242 
7243 
7244 
7245 
7246 
7247 
7248 
7249 
