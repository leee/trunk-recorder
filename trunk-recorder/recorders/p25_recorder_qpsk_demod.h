#ifndef P25_RECORDER_QPSK_DEMOD_H
#define P25_RECORDER_QPSK_DEMOD_H

#include <gnuradio/analog/feedforward_agc_cc.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/complex_to_arg.h>
#include <gnuradio/digital/diff_phasor_cc.h>
#include <gnuradio/filter/fft_filter_ccf.h>
#include <gnuradio/filter/fft_filter_fff.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/msg_queue.h>
#include <op25_repeater/gardner_costas_cc.h>

#include <boost/shared_ptr.hpp>

#if GNURADIO_VERSION < 0x030800
#include <gnuradio/blocks/multiply_const_ff.h>
#include <gnuradio/filter/fir_filter_fff.h>
#else
#include <gnuradio/blocks/multiply_const.h>
#include <gnuradio/filter/fir_filter_blk.h>
#endif

class p25_recorder_qpsk_demod;
typedef boost::shared_ptr<p25_recorder_qpsk_demod> p25_recorder_qpsk_demod_sptr;
p25_recorder_qpsk_demod_sptr make_p25_recorder_qpsk_demod();

class p25_recorder_qpsk_demod : public gr::hier_block2 {
  friend p25_recorder_qpsk_demod_sptr make_p25_recorder_qpsk_demod();

 protected:
  virtual void initialize();

  gr::op25_repeater::gardner_costas_cc::sptr costas_clock;

 public:
  p25_recorder_qpsk_demod();
  virtual ~p25_recorder_qpsk_demod();
  void switch_tdma(bool phase2);
  void reset();

 private:
  double system_channel_rate;
  double symbol_rate;
  int samples_per_symbol;
  const int phase1_samples_per_symbol = 5;
  const int phase2_samples_per_symbol = 4;
  const double phase1_symbol_rate = 4800;
  const double phase2_symbol_rate = 6000;

  std::vector<float> baseband_noise_filter_taps;
  std::vector<float> sym_taps;

  std::vector<float> cutoff_filter_coeffs;
  gr::filter::fft_filter_fff::sptr noise_filter;
  gr::filter::fir_filter_fff::sptr sym_filter;
  gr::analog::feedforward_agc_cc::sptr agc;
  gr::digital::diff_phasor_cc::sptr diffdec;
  gr::blocks::complex_to_arg::sptr to_float;
  gr::blocks::multiply_const_ff::sptr rescale;
};
#endif