
//  $Id: front_end.C,v 1.2 2016/01/23 03:15:23 stanchen Exp $

#include "front_end.H"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** Module for doing windowing. **/
void FrontEnd::do_window(const matrix<double>& in_feats,
                         matrix<double>& out_feats) const {
  //  Get parameters.
  //  Input samples per second.
  double sample_rate = get_float_param(params_, "window.sample_rate", 20000.0);
  //  Output frames per second.
  double frames_per_sec =
      get_float_param(params_, "window.frames_per_sec", 100.0);
  //  Width of each window, in seconds.
  double window_width = get_float_param(params_, "window.window_size", 0.025);
  //  Whether to do Hamming or rectangular windowing.
  bool do_Hamming = get_bool_param(params_, "window.hamming", true);

  //  Get number of input samples.
  int in_samp_cnt = in_feats.size1();
  if (in_feats.size2() != 1)
    throw runtime_error("Windowing expected vector input.");

  //  Input sampling period in seconds.
  double sample_period = 1.0 / sample_rate;
  //  Output frame period, in seconds.
  double frame_period = 1.0 / frames_per_sec;
  //  Number of samples per window.
  int samp_per_window = (int)(window_width / sample_period + 0.5);
  //  Number of samples to shift between each window.
  int samp_shift = (int)(frame_period / sample_period + 0.5);
  //  Number of output frames.
  int out_frame_cnt = (in_samp_cnt - samp_per_window) / samp_shift + 1;

  //  Allocate output matrix and fill with zeros.
  out_feats.resize(out_frame_cnt, samp_per_window);
  out_feats.clear();

  //  BEGIN_LAB
  //
  //  Input:
  //      "in_feats", a matrix containing a single column holding the
  //      input samples for an utterance.  Each row holds a single sample.
  //
  //      in_feats(0 .. (in_samp_cnt - 1), 0)
  //
  //  Output:
  //      "out_feats", which should contain the result of windowing.
  //
  //      out_feats(0 .. (out_frame_cnt - 1), 0 .. (samp_per_window - 1))
  //
  //      Each row corresponds to a frame, and should hold the
  //      windowed samples for that frame.
  //      It has already been allocated to be of the correct size.
  //      If the boolean "do_Hamming" is true, then a Hamming
  //      window should be applied; otherwise, a rectangular
  //      window should be used.
  //
  //  See "in_samp_cnt", "samp_per_window", "samp_shift", and "out_frame_cnt"
  //  above for quantities you may (or may not) need for this computation.
  //
  //  When accessing matrices such as "in_feats" and "out_feats",
  //  use a syntax like "in_feats(frm_idx, dim_idx)" to access elements;
  //  using square brackets as in normal C arrays won't work.

  // cout << format("samp_shift %d\n") % samp_shift;
  // cout << format("out_frame_cnt %d\n") % out_frame_cnt;
  // cout << format("samp_perWindw %d\n") % samp_per_window;
  // cout << format("in_samp_cnt %d\n") % in_samp_cnt;
  if (do_Hamming) {
    // Hamming windows
    matrix<double> hamming_window;
    hamming_window.resize(samp_per_window, 1);
    for (int ii = 0; ii < samp_per_window; ++ii) {
      hamming_window(ii, 0) = 0.54f - 0.46f * cos(2.0f * PI * ii / (samp_per_window - 1));
    }
    int in_start_idx = 0;
    for (int out_idx = 0; out_idx < out_frame_cnt; ++out_idx) {
      for (int ii = 0; ii < samp_per_window; ++ii) {
        out_feats(out_idx, ii) = in_feats(in_start_idx + ii, 0) * hamming_window(ii, 0); 
      }
      in_start_idx += samp_shift;
    }    
  } else {
    int in_start_idx = 0;
    for (int out_idx = 0; out_idx < out_frame_cnt; ++out_idx) {
      for (int ii = 0; ii < samp_per_window; ++ii) {
        out_feats(out_idx, ii) = in_feats(in_start_idx + ii, 0);
      }
      in_start_idx += samp_shift;
    }
    // Rectangular window
  }
  //  END_LAB
}

/** Module for doing FFT. **/
void FrontEnd::do_fft(const matrix<double>& in_feats,
                      matrix<double>& out_feats) const {
  //  Make output dimension the smallest power of 2 at least as
  //  large as input dimension.
  int in_frame_cnt = in_feats.size1();
  int in_dim_cnt = in_feats.size2();
  int out_dim_cnt = 2;
  while (out_dim_cnt < in_dim_cnt) out_dim_cnt *= 2;

  //  Allocate output matrix and fill with zeros.
  out_feats.resize(in_frame_cnt, out_dim_cnt);
  out_feats.clear();

  //  Input:
  //      "in_feats", a matrix with each row holding the windowed
  //      values for that frame.
  //
  //      in_feats(0 .. (in_frame_cnt - 1), 0 .. (in_dim_cnt - 1))
  //
  //  Output:
  //      "out_feats", where an FFT should be applied to each
  //      row/frame of "in_feats".
  //
  //      out_feats(0 .. (in_frame_cnt - 1), 0 .. (out_dim_cnt - 1))
  //
  //      For a given row/frame "frm_idx", the real and imaginary
  //      parts of the FFT value for frequency i/(out_dim_cnt*T)
  //      where T is the sample period are held in
  //      out_feats(frm_idx, 2*i) and out_feats(frm_idx, 2*i+1),
  //      respectively.

  vector<double> fft_buf;
  for (int frm_idx = 0; frm_idx < in_frame_cnt; ++frm_idx) {
    copy_matrix_row_to_vector(in_feats, frm_idx, fft_buf);
    //  Pad window with zeros, if needed.
    fft_buf.resize(out_dim_cnt, 0.0);
    real_fft(fft_buf);
    copy_vector_to_matrix_row(fft_buf, out_feats, frm_idx);
  }
}

/** Module for mel binning. **/
// change to google name style
void FrontEnd::do_melbin(const matrix<double>& in_feats,
                         matrix<double>& out_feats) const {
  //  Number of mel bins to make.
  int num_bins = get_int_param(params_, "melbin.bins", 26);
  //  Whether to take log of output or not.
  bool do_log = get_bool_param(params_, "melbin.log", true);
  //  Input samples per second.
  double sample_rate = get_float_param(params_, "window.sample_rate", 20000.0);
  double sample_period = 1.0 / sample_rate;

  //  Retrieve number of frames and dimension of input feature vectors.
  int in_frame_cnt = in_feats.size1();
  int in_dim_cnt = in_feats.size2();
  int out_dim_cnt = num_bins;

  //  Allocate output matrix and fill with zeros.
  out_feats.resize(in_frame_cnt, out_dim_cnt);
  out_feats.clear();

  //  BEGIN_LAB
  //
  //  Input:
  //      "in_feats", holding the output of a real FFT.
  //
  //      in_feats(0 .. (in_frame_cnt - 1), 0 .. (in_dim_cnt - 1))
  //
  //  Output:
  //      "out_feats", which should contain the result of
  //      mel-binning.
  //
  //      out_feats(0 .. (in_frame_cnt - 1), 0 .. (out_dim_cnt - 1))
  //
  //      If the boolean "doLog" is true,
  //      then each value should be replaced with its natural
  //      logarithm, or 0 if its logarithm is negative.
  //      "out_feats" has been allocated to be of the correct size.
  //
  //  See "in_frame_cnt", "in_dim_cnt", "out_dim_cnt", and "sample_period"
  //  above for quantities you will need for this computation.

  // cout << format("sample_period %ls\n") % sample_period;
  // cout << format("input dim %d\n") % in_dim_cnt;

  // TODO: Optimize the compuatation order to avoid duplicate computation
  matrix<double> mel_bins;
  double f_mel_max = freq_to_mel(0.5f / sample_period);
  gen_mel_bins(mel_bins, f_mel_max, num_bins, in_dim_cnt, sample_period);
    
  //cout << format("generate mel bins done!\n");

  for (int ii = 0; ii < in_frame_cnt; ++ii) {
    for (int jj = 0; jj < out_dim_cnt; ++jj) {
      out_feats(ii, jj) = 0.0f;
      for (int kk = 0; kk < in_dim_cnt / 2; ++kk) {
        double Xf = cal_magnitude(in_feats(ii, 2 * kk), in_feats(ii, 2 * kk + 1));
        out_feats(ii, jj) += Xf * mel_bins(jj, kk);
      }
      if (do_log) {
        out_feats(ii, jj) = log(out_feats(ii, jj));
      }
    }
  }

  //  END_LAB
}

/** Module for doing discrete cosine transform. **/
// change to google name style
void FrontEnd::do_dct(const matrix<double>& in_feats,
                      matrix<double>& out_feats) const {
  //  Number of DCT coefficients to output.
  int num_coeffs = get_int_param(params_, "dct.coeffs", 12);
  int in_frame_cnt = in_feats.size1();
  int in_dim_cnt = in_feats.size2();
  int out_dim_cnt = num_coeffs;

  //  Allocate output matrix and fill with zeros.
  out_feats.resize(in_frame_cnt, out_dim_cnt);
  out_feats.clear();

  //  BEGIN_LAB
  //
  //  Input:
  //      The matrix "in_feats", holding the output of mel-binning.
  //
  //      in_feats(0 .. (in_frame_cnt - 1), 0 .. (in_dim_cnt - 1))
  //
  //  Output:
  //      The matrix "out_feats", which should contain the result of
  //      applying the DCT.
  //
  //      out_feats(0 .. (in_frame_cnt - 1), 0 .. (out_dim_cnt - 1))
  //
  //      "out_feats" has been allocated to be of the correct size.
  //
  //  See "in_frame_cnt", "in_dim_cnt", and "out_dim_cnt" above
  //  for quantities you will need for this computation.
  
  //  END_LAB
}

/** Main signal processing routine.
 *   Calls each signal processing module in turn, unless
 *   parameter says not to.
 **/
void FrontEnd::get_feats(const matrix<double>& inAudio,
                         matrix<double>& out_feats) const {
  if (get_bool_param(params_, "frontend.null", false)) {
    out_feats = inAudio;
    return;
  }
  matrix<double> curFeats(inAudio);
  if (get_bool_param(params_, "frontend.window", true)) {
    do_window(curFeats, out_feats);
    out_feats.swap(curFeats);
  }
  if (get_bool_param(params_, "frontend.fft", true)) {
    do_fft(curFeats, out_feats);
    out_feats.swap(curFeats);
  }
  if (get_bool_param(params_, "frontend.melbin", true)) {
    do_melbin(curFeats, out_feats);
    out_feats.swap(curFeats);
  }
  if (get_bool_param(params_, "frontend.dct", true)) {
    do_dct(curFeats, out_feats);
    out_feats.swap(curFeats);
  }
  out_feats.swap(curFeats);
}

/** Generate mel bins **/
void FrontEnd::gen_mel_bins(matrix<double>& mel_bins,
                            const double f_mel_max,
                            const int num_bins,
                            const int in_dim,
                            const double sample_period) const {
  double df = f_mel_max / (num_bins + 1);
  double slope = 1.0f / df;
  mel_bins.resize(num_bins, in_dim / 2);
  //cout << format("mel_bins size : %d, %d\n") % mel_bins.size1() % mel_bins.size2();
  for (int ii = 0; ii < num_bins; ++ii) {
    for (int jj = 0; jj < in_dim / 2; ++jj) {
      double f_mel = freq_to_mel(jj / (in_dim * sample_period));
      if (f_mel < ii * df || f_mel > (ii + 2) * df) {
        mel_bins(ii, jj) = 0.0f;
      } else if (f_mel >= ii * df && f_mel <= (ii + 1) * df) {
        mel_bins(ii, jj) = slope * f_mel - ii;
      } else if (f_mel > (ii + 1) * df && f_mel <= (ii + 2) * df) {
        mel_bins(ii, jj) = -1.0f * slope * f_mel + (ii + 2);
      } else {
        cout << format("mel freq range error : f_mel_max = %f, cur_mel_f = %f\n") % f_mel_max % f_mel;
      }
    }
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
