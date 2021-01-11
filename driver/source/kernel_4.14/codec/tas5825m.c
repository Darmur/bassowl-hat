/*
 * Driver for the TAS5825M Audio Amplifier
 *
 * Author: Andy Liu <andy-liu@ti.com>
 * Author: Dario Murgia <dario.murgia@alumni.uniroma2.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/regmap.h>

#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/initval.h>

#include "tas5825m.h"

#define TAS5825M_DRV_NAME    "tas5825m"

#define TAS5825M_RATES	     (SNDRV_PCM_RATE_8000_192000)
#define TAS5825M_FORMATS     (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
			                  SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

#define TAS5825M_REG_00      (0x00)
#define TAS5825M_REG_03      (0x03)
#define TAS5825M_REG_0C      (0x0c)
#define TAS5825M_REG_0D      (0x0d)
#define TAS5825M_REG_0E      (0x0e)
#define TAS5825M_REG_0F      (0x0f)
#define TAS5825M_REG_10      (0x10)
#define TAS5825M_REG_11      (0x11)
#define TAS5825M_REG_12      (0x12)
#define TAS5825M_REG_13      (0x13)
#define TAS5825M_REG_35      (0x35)
#define TAS5825M_REG_7F      (0x7f)

#define TAS5825M_PAGE_00     (0x00)
#define TAS5825M_PAGE_0B     (0x0b)

#define TAS5825M_BOOK_00     (0x00)
#define TAS5825M_BOOK_8C     (0x8c)

#define TAS5825M_VOLUME_MAX  (110) //158
#define TAS5825M_VOLUME_MIN  (40) //0

const uint32_t tas5825m_volume[] = {
	0x0000001B,    //0, -110dB
	0x0000001E,    //1, -109dB
	0x00000021,    //2, -108dB
	0x00000025,    //3, -107dB
	0x0000002A,    //4, -106dB
	0x0000002F,    //5, -105dB
	0x00000035,    //6, -104dB
	0x0000003B,    //7, -103dB
	0x00000043,    //8, -102dB
	0x0000004B,    //9, -101dB
	0x00000054,    //10, -100dB
	0x0000005E,    //11, -99dB
	0x0000006A,    //12, -98dB
	0x00000076,    //13, -97dB
	0x00000085,    //14, -96dB
	0x00000095,    //15, -95dB
	0x000000A7,    //16, -94dB
	0x000000BC,    //17, -93dB
	0x000000D3,    //18, -92dB
	0x000000EC,    //19, -91dB
	0x00000109,    //20, -90dB
	0x0000012A,    //21, -89dB
	0x0000014E,    //22, -88dB
	0x00000177,    //23, -87dB
	0x000001A4,    //24, -86dB
	0x000001D8,    //25, -85dB
	0x00000211,    //26, -84dB
	0x00000252,    //27, -83dB
	0x0000029A,    //28, -82dB
	0x000002EC,    //29, -81dB
	0x00000347,    //30, -80dB
	0x000003AD,    //31, -79dB
	0x00000420,    //32, -78dB
	0x000004A1,    //33, -77dB
	0x00000532,    //34, -76dB
	0x000005D4,    //35, -75dB
	0x0000068A,    //36, -74dB
	0x00000756,    //37, -73dB
	0x0000083B,    //38, -72dB
	0x0000093C,    //39, -71dB
	0x00000A5D,    //40, -70dB
	0x00000BA0,    //41, -69dB
	0x00000D0C,    //42, -68dB
	0x00000EA3,    //43, -67dB
	0x0000106C,    //44, -66dB
	0x0000126D,    //45, -65dB
	0x000014AD,    //46, -64dB
	0x00001733,    //47, -63dB
	0x00001A07,    //48, -62dB
	0x00001D34,    //49, -61dB
	0x000020C5,    //50, -60dB
	0x000024C4,    //51, -59dB
	0x00002941,    //52, -58dB
	0x00002E49,    //53, -57dB
	0x000033EF,    //54, -56dB
	0x00003A45,    //55, -55dB
	0x00004161,    //56, -54dB
	0x0000495C,    //57, -53dB
	0x0000524F,    //58, -52dB
	0x00005C5A,    //59, -51dB
	0x0000679F,    //60, -50dB
	0x00007444,    //61, -49dB
	0x00008274,    //62, -48dB
	0x0000925F,    //63, -47dB
	0x0000A43B,    //64, -46dB
	0x0000B845,    //65, -45dB
	0x0000CEC1,    //66, -44dB
	0x0000E7FB,    //67, -43dB
	0x00010449,    //68, -42dB
	0x0001240C,    //69, -41dB
	0x000147AE,    //70, -40dB
	0x00016FAA,    //71, -39dB
	0x00019C86,    //72, -38dB
	0x0001CEDC,    //73, -37dB
	0x00020756,    //74, -36dB
	0x000246B5,    //75, -35dB
	0x00028DCF,    //76, -34dB
	0x0002DD96,    //77, -33dB
	0x00033718,    //78, -32dB
	0x00039B87,    //79, -31dB
	0x00040C37,    //80, -30dB
	0x00048AA7,    //81, -29dB
	0x00051884,    //82, -28dB
	0x0005B7B1,    //83, -27dB
	0x00066A4A,    //84, -26dB
	0x000732AE,    //85, -25dB
	0x00081385,    //86, -24dB
	0x00090FCC,    //87, -23dB
	0x000A2ADB,    //88, -22dB
	0x000B6873,    //89, -21dB
	0x000CCCCD,    //90, -20dB
	0x000E5CA1,    //91, -19dB
	0x00101D3F,    //92, -18dB
	0x0012149A,    //93, -17dB
	0x00144961,    //94, -16dB
	0x0016C311,    //95, -15dB
	0x00198A13,    //96, -14dB
	0x001CA7D7,    //97, -13dB
	0x002026F3,    //98, -12dB
	0x00241347,    //99, -11dB
	0x00287A27,    //100, -10dB
	0x002D6A86,    //101, -9dB
	0x0032F52D,    //102, -8dB
	0x00392CEE,    //103, -7dB
	0x004026E7,    //104, -6dB
	0x0047FACD,    //105, -5dB
	0x0050C336,    //106, -4dB
	0x005A9DF8,    //107, -3dB
	0x0065AC8C,    //108, -2dB
	0x00721483,    //109, -1dB
	0x00800000,    //110, 0dB
	0x008F9E4D,    //111, 1dB
	0x00A12478,    //112, 2dB
	0x00B4CE08,    //113, 3dB
	0x00CADDC8,    //114, 4dB
	0x00E39EA9,    //115, 5dB
	0x00FF64C1,    //116, 6dB
	0x011E8E6A,    //117, 7dB
	0x0141857F,    //118, 8dB
	0x0168C0C6,    //119, 9dB
	0x0194C584,    //120, 10dB
	0x01C62940,    //121, 11dB
	0x01FD93C2,    //122, 12dB
	0x023BC148,    //123, 13dB
	0x02818508,    //124, 14dB
	0x02CFCC01,    //125, 15dB
	0x0327A01A,    //126, 16dB
	0x038A2BAD,    //127, 17dB
	0x03F8BD7A,    //128, 18dB
	0x0474CD1B,    //129, 19dB
	0x05000000,    //130, 20dB
	0x059C2F02,    //131, 21dB
	0x064B6CAE,    //132, 22dB
	0x07100C4D,    //133, 23dB
	0x07ECA9CD,    //134, 24dB
	0x08E43299,    //135, 25dB
	0x09F9EF8E,    //136, 26dB
	0x0B319025,    //137, 27dB
	0x0C8F36F2,    //138, 28dB
	0x0E1787B8,    //139, 29dB
	0x0FCFB725,    //140, 30dB
	0x11BD9C84,    //141, 31dB
	0x13E7C594,    //142, 32dB
	0x16558CCB,    //143, 33dB
	0x190F3254,    //144, 34dB
	0x1C1DF80E,    //145, 35dB
	0x1F8C4107,    //146, 36dB
	0x2365B4BF,    //147, 37dB
	0x27B766C2,    //148, 38dB
	0x2C900313,    //149, 39dB
	0x32000000,    //150, 40dB
	0x3819D612,    //151, 41dB
	0x3EF23ECA,    //152, 42dB
	0x46A07B07,    //153, 43dB
	0x4F3EA203,    //154, 44dB
	0x58E9F9F9,    //155, 45dB
	0x63C35B8E,    //156, 46dB
	0x6FEFA16D,    //157, 47dB
	0x7D982575,    //158, 48dB
};

struct tas5825m_priv {
	struct regmap *regmap;

	struct mutex lock;

	int vol;
};

const struct regmap_config tas5825m_regmap = {
	.reg_bits = 8,
	.val_bits = 8,
	.cache_type = REGCACHE_RBTREE,
};

static int tas5825m_vol_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type   = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->access = (SNDRV_CTL_ELEM_ACCESS_TLV_READ | SNDRV_CTL_ELEM_ACCESS_READWRITE);
	uinfo->count  = 1;

	uinfo->value.integer.min  = TAS5825M_VOLUME_MIN;
	uinfo->value.integer.max  = TAS5825M_VOLUME_MAX;
	uinfo->value.integer.step = 1;

	return 0;
}

static int tas5825m_vol_locked_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_soc_kcontrol_codec(kcontrol);
	struct tas5825m_priv *tas5825m = snd_soc_codec_get_drvdata(codec);

	mutex_lock(&tas5825m->lock);
	ucontrol->value.integer.value[0] = tas5825m->vol;
	mutex_unlock(&tas5825m->lock);

	return 0;
}

static inline int get_volume_index(int vol)
{
	int index;
	index = vol;

	if (index < TAS5825M_VOLUME_MIN)
		index = TAS5825M_VOLUME_MIN;

	if (index > TAS5825M_VOLUME_MAX)
		index = TAS5825M_VOLUME_MAX;

	return index;
}

static void tas5825m_set_volume(struct snd_soc_codec *codec, int vol)
{
	unsigned int index;
	uint32_t volume_hex;
	uint8_t byte4;
	uint8_t byte3;
	uint8_t byte2;
	uint8_t byte1;

	index = get_volume_index(vol);
	volume_hex = tas5825m_volume[index];

	byte4 = ((volume_hex >> 24) & 0xFF);
	byte3 = ((volume_hex >> 16) & 0xFF);
	byte2 = ((volume_hex >> 8)	& 0xFF);
	byte1 = ((volume_hex >> 0)	& 0xFF);

	snd_soc_write(codec, TAS5825M_REG_00, TAS5825M_PAGE_00);
	snd_soc_write(codec, TAS5825M_REG_7F, TAS5825M_BOOK_8C);
	snd_soc_write(codec, TAS5825M_REG_00, TAS5825M_PAGE_0B);
	snd_soc_write(codec, TAS5825M_REG_0C, byte4);
	snd_soc_write(codec, TAS5825M_REG_0D, byte3);
	snd_soc_write(codec, TAS5825M_REG_0E, byte2);
	snd_soc_write(codec, TAS5825M_REG_0F, byte1);
	snd_soc_write(codec, TAS5825M_REG_10, byte4);
	snd_soc_write(codec, TAS5825M_REG_11, byte3);
	snd_soc_write(codec, TAS5825M_REG_12, byte2);
	snd_soc_write(codec, TAS5825M_REG_13, byte1);
}

static int tas5825m_vol_locked_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_soc_kcontrol_codec(kcontrol);
	struct tas5825m_priv *tas5825m = snd_soc_codec_get_drvdata(codec);

	mutex_lock(&tas5825m->lock);

	tas5825m->vol = ucontrol->value.integer.value[0];
	tas5825m_set_volume(codec, tas5825m->vol);

	mutex_unlock(&tas5825m->lock);

	return 0;
}

static const struct snd_kcontrol_new tas5825m_vol_control =
{
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name  = "Master Playback Volume",
	.info  = tas5825m_vol_info,
	.get   = tas5825m_vol_locked_get,
	.put   = tas5825m_vol_locked_put,
};

static int tas5825m_snd_probe(struct snd_soc_codec *codec)
{
	int ret;

	ret = snd_soc_add_codec_controls(codec, &tas5825m_vol_control, 1);

	return ret;
}

static struct snd_soc_codec_driver soc_codec_tas5825m = {
	.probe = tas5825m_snd_probe,
};

static int tas5825m_mute(struct snd_soc_dai *dai, int mute)
{
	u8 reg03_value = 0;
	u8 reg35_value = 0;
	struct snd_soc_codec *codec = dai->codec;

	if (mute)
	{
		reg03_value = 0x0b;
		reg35_value = 0x00;
	}
	else
	{
		reg03_value = 0x03;
		reg35_value = 0x11;
	}

	snd_soc_write(codec, TAS5825M_REG_00, TAS5825M_PAGE_00);
	snd_soc_write(codec, TAS5825M_REG_7F, TAS5825M_BOOK_00);
	snd_soc_write(codec, TAS5825M_REG_00, TAS5825M_PAGE_00);
	snd_soc_write(codec, TAS5825M_REG_03, reg03_value);
	snd_soc_write(codec, TAS5825M_REG_35, reg35_value);

	return 0;
}

static const struct snd_soc_dai_ops tas5825m_dai_ops = {
	.digital_mute = tas5825m_mute,
};

static struct snd_soc_dai_driver tas5825m_dai = {
	.name		= "tas5825m-amp",
	.playback 	= {
		.stream_name	= "Playback",
		.channels_min	= 2,
		.channels_max	= 2,
		.rates		= TAS5825M_RATES,
		.formats	= TAS5825M_FORMATS,
	},
	.ops = &tas5825m_dai_ops,
};

static int tas5825m_probe(struct device *dev, struct regmap *regmap)
{
	struct tas5825m_priv *tas5825m;
	int ret;

	tas5825m = devm_kzalloc(dev, sizeof(struct tas5825m_priv), GFP_KERNEL);
	if (!tas5825m)
		return -ENOMEM;

	dev_set_drvdata(dev, tas5825m);
	tas5825m->regmap = regmap;
	tas5825m->vol    = 85;         // -25dB

	mutex_init(&tas5825m->lock);

	ret = regmap_register_patch(regmap, tas5825m_init_sequence, ARRAY_SIZE(tas5825m_init_sequence));
	if (ret != 0)
	{
		dev_err(dev, "Failed to initialize TAS5825M: %d\n",ret);
		goto err;

	}

	ret = snd_soc_register_codec(dev,
	                             &soc_codec_tas5825m,
			                     &tas5825m_dai,
								 1);
	if (ret != 0)
	{
		dev_err(dev, "Failed to register CODEC: %d\n", ret);
		goto err;
	}

	return 0;

err:
	return ret;

}

static int tas5825m_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
	struct regmap *regmap;
	struct regmap_config config = tas5825m_regmap;

	regmap = devm_regmap_init_i2c(i2c, &config);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	return tas5825m_probe(&i2c->dev, regmap);
}

static int tas5825m_remove(struct device *dev)
{
	snd_soc_unregister_codec(dev);

	return 0;
}

static int tas5825m_i2c_remove(struct i2c_client *i2c)
{
	tas5825m_remove(&i2c->dev);

	return 0;
}

static const struct i2c_device_id tas5825m_i2c_id[] = {
	{ "tas5825m", },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tas5825m_i2c_id);

#ifdef CONFIG_OF
static const struct of_device_id tas5825m_of_match[] = {
	{ .compatible = "ti,tas5825m", },
	{ }
};
MODULE_DEVICE_TABLE(of, tas5825m_of_match);
#endif

static struct i2c_driver tas5825m_i2c_driver = {
	.probe 		= tas5825m_i2c_probe,
	.remove 	= tas5825m_i2c_remove,
	.id_table	= tas5825m_i2c_id,
	.driver		= {
		.name	= TAS5825M_DRV_NAME,
		.of_match_table = tas5825m_of_match,
	},
};

module_i2c_driver(tas5825m_i2c_driver);

MODULE_AUTHOR("Andy Liu <andy-liu@ti.com>");
MODULE_AUTHOR("Dario Murgia <dario.murgia@alumni.uniroma2.eu>");
MODULE_DESCRIPTION("TAS5825M Audio Amplifier Driver");
MODULE_LICENSE("GPL v2");
