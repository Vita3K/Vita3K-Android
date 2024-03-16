/*
 * Copyright 2013 Dolphin Emulator Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

package org.vita3k.emulator.overlay;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.view.MotionEvent;
/**
 * Custom {@link BitmapDrawable} that is capable
 * of storing it's own ID.
 */
public final class InputOverlayDrawableJoystick
{
  private float mCurrentX = 0.0f;
  private float mCurrentY = 0.0f;
  private float mJoystickX = 0.0f;
  private float mJoystickY = 0.0f;
  private int trackId = -1;
  private final int mJoystickLegacyId;
  private final int mJoystickXControl;
  private final int mJoystickYControl;
  private int mControlPositionX, mControlPositionY;
  private int mPreviousTouchX, mPreviousTouchY;
  private final int mWidth;
  private final int mHeight;
  private Rect mVirtBounds;
  private Rect mOrigBounds;
  private int mOpacity;
  private final BitmapDrawable mOuterBitmap;
  private final BitmapDrawable mDefaultStateInnerBitmap;
  private final BitmapDrawable mPressedStateInnerBitmap;
  private final BitmapDrawable mBoundsBoxBitmap;
  private boolean mPressedState = false;

  /**
   * Constructor
   *
   * @param res                {@link Resources} instance.
   * @param bitmapOuter        {@link Bitmap} which represents the outer non-movable part of the joystick.
   * @param bitmapInnerDefault {@link Bitmap} which represents the default inner movable part of the joystick.
   * @param bitmapInnerPressed {@link Bitmap} which represents the pressed inner movable part of the joystick.
   * @param rectOuter          {@link Rect} which represents the outer joystick bounds.
   * @param rectInner          {@link Rect} which represents the inner joystick bounds.
   * @param legacyId           Legacy identifier (ButtonType) for which joystick this is.
   * @param xControl           The control which the x value of the joystick will be written to.
   * @param yControl           The control which the y value of the joystick will be written to.
   */
  public InputOverlayDrawableJoystick(Resources res, Bitmap bitmapOuter, Bitmap bitmapInnerDefault,
          Bitmap bitmapInnerPressed, Rect rectOuter, Rect rectInner, int legacyId, int xControl,
          int yControl)
  {
    mJoystickLegacyId = legacyId;
    mJoystickXControl = xControl;
    mJoystickYControl = yControl;

    mOuterBitmap = new BitmapDrawable(res, bitmapOuter);
    mDefaultStateInnerBitmap = new BitmapDrawable(res, bitmapInnerDefault);
    mPressedStateInnerBitmap = new BitmapDrawable(res, bitmapInnerPressed);
    mBoundsBoxBitmap = new BitmapDrawable(res, bitmapOuter);
    mWidth = bitmapOuter.getWidth();
    mHeight = bitmapOuter.getHeight();

    setBounds(rectOuter);
    mDefaultStateInnerBitmap.setBounds(rectInner);
    mPressedStateInnerBitmap.setBounds(rectInner);
    mVirtBounds = getBounds();
    mOrigBounds = mOuterBitmap.copyBounds();
    mBoundsBoxBitmap.setAlpha(0);
    mBoundsBoxBitmap.setBounds(getVirtBounds());
    SetInnerBounds();
  }

  /**
   * Gets this InputOverlayDrawableJoystick's legacy ID.
   *
   * @return this InputOverlayDrawableJoystick's legacy ID.
   */
  public int getLegacyId()
  {
    return mJoystickLegacyId;
  }

  public void draw(Canvas canvas)
  {
    mOuterBitmap.draw(canvas);
    getCurrentStateBitmapDrawable().draw(canvas);
    mBoundsBoxBitmap.draw(canvas);
  }

  public boolean TrackEvent(MotionEvent event)
  {
    boolean reCenter = false;
    int action = event.getActionMasked();
    boolean firstPointer = action != MotionEvent.ACTION_POINTER_DOWN &&
            action != MotionEvent.ACTION_POINTER_UP;
    int pointerIndex = firstPointer ? 0 : event.getActionIndex();
    boolean concerned = false;

    switch (action)
    {
      case MotionEvent.ACTION_DOWN:
      case MotionEvent.ACTION_POINTER_DOWN:
        if (getBounds().contains((int) event.getX(pointerIndex), (int) event.getY(pointerIndex)))
        {
          concerned = true;
          mPressedState = true;
          mOuterBitmap.setAlpha(0);
          mBoundsBoxBitmap.setAlpha(mOpacity);
          if (reCenter)
          {
            getVirtBounds().offset((int) event.getX(pointerIndex) - getVirtBounds().centerX(),
                    (int) event.getY(pointerIndex) - getVirtBounds().centerY());
          }
          mBoundsBoxBitmap.setBounds(getVirtBounds());
          trackId = event.getPointerId(pointerIndex);
        }
        break;
      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_POINTER_UP:
        if (trackId == event.getPointerId(pointerIndex))
        {
          concerned = true;
          mPressedState = false;
          mCurrentX = mCurrentY = 0.0f;
          mJoystickX = mJoystickY = 0.0f;
          mOuterBitmap.setAlpha(mOpacity);
          mBoundsBoxBitmap.setAlpha(0);
          setVirtBounds(new Rect(mOrigBounds.left, mOrigBounds.top, mOrigBounds.right,
                  mOrigBounds.bottom));
          setBounds(new Rect(mOrigBounds.left, mOrigBounds.top, mOrigBounds.right,
                  mOrigBounds.bottom));
          SetInnerBounds();
          trackId = -1;
        }
        break;
    }

    if (trackId == -1)
      return concerned;

    for (int i = 0; i < event.getPointerCount(); i++)
    {
      if (trackId == event.getPointerId(i))
      {
        concerned = true;
        float touchX = event.getX(i);
        float touchY = event.getY(i);
        float maxY = getVirtBounds().bottom;
        float maxX = getVirtBounds().right;
        touchX -= getVirtBounds().centerX();
        maxX -= getVirtBounds().centerX();
        touchY -= getVirtBounds().centerY();
        maxY -= getVirtBounds().centerY();
        mCurrentX = touchX / maxX;
        mCurrentY = touchY / maxY;

        SetInnerBounds();
      }
    }
    return concerned;
  }

  public void onConfigureTouch(MotionEvent event)
  {
    switch (event.getAction())
    {
      case MotionEvent.ACTION_DOWN:
        mPreviousTouchX = (int) event.getX();
        mPreviousTouchY = (int) event.getY();
        break;
      case MotionEvent.ACTION_MOVE:
        int deltaX = (int) event.getX() - mPreviousTouchX;
        int deltaY = (int) event.getY() - mPreviousTouchY;
        mControlPositionX += deltaX;
        mControlPositionY += deltaY;
        setBounds(new Rect(mControlPositionX, mControlPositionY,
                mOuterBitmap.getIntrinsicWidth() + mControlPositionX,
                mOuterBitmap.getIntrinsicHeight() + mControlPositionY));
        setVirtBounds(new Rect(mControlPositionX, mControlPositionY,
                mOuterBitmap.getIntrinsicWidth() + mControlPositionX,
                mOuterBitmap.getIntrinsicHeight() + mControlPositionY));
        SetInnerBounds();
        setOrigBounds(new Rect(new Rect(mControlPositionX, mControlPositionY,
                mOuterBitmap.getIntrinsicWidth() + mControlPositionX,
                mOuterBitmap.getIntrinsicHeight() + mControlPositionY)));
        mPreviousTouchX = (int) event.getX();
        mPreviousTouchY = (int) event.getY();
        break;
    }
  }

  public float getX()
  {
    return mJoystickX;
  }

  public float getY()
  {
    return mJoystickY;
  }

  public int getXControl()
  {
    return mJoystickXControl;
  }

  public int getYControl()
  {
    return mJoystickYControl;
  }

  private void SetInnerBounds()
  {
    double x = mCurrentX;
    double y = mCurrentY;

    double angle = Math.atan2(y, x) + Math.PI + Math.PI;
    double radius = Math.hypot(y, x);
    double maxRadius = 1.0f;
    if (radius > maxRadius)
    {
      y = maxRadius * Math.sin(angle);
      x = maxRadius * Math.cos(angle);
      mCurrentY = (float) y;
      mCurrentX = (float) x;
    }

    int pixelX = getVirtBounds().centerX() + (int) (x * (getVirtBounds().width() / 2));
    int pixelY = getVirtBounds().centerY() + (int) (y * (getVirtBounds().height() / 2));

    int width = mPressedStateInnerBitmap.getBounds().width() / 2;
    int height = mPressedStateInnerBitmap.getBounds().height() / 2;
    mDefaultStateInnerBitmap.setBounds(pixelX - width, pixelY - height, pixelX + width,
            pixelY + height);
    mPressedStateInnerBitmap.setBounds(mDefaultStateInnerBitmap.getBounds());

    // x,y is projected onto a circle while we want the values to be projected on the bounding square
    // for example, when (x,y) = (1/sqrt(2), 1/sqrt(2)) we want to set (joyX, joyY) to (1,1)
    x = Math.abs(x);
    y = Math.abs(y);
    if(x < y){
      double temp = x;
      x = y;
      y = temp;
    }
    if(x < 0.01){
      // too centered, scaling does not make sense
      mJoystickX = mCurrentX;
      mJoystickY = mCurrentY;
    } else {
      // (x,y) projected on the bounding square is (1, y_proj)
      float y_proj = (float)(y / x);
      float scaling = (float)Math.sqrt(1.0f + y_proj * y_proj);
      mJoystickX = mCurrentX * scaling;
      mJoystickY = mCurrentY * scaling;
    }
  }

  public void setPosition(int x, int y)
  {
    mControlPositionX = x;
    mControlPositionY = y;
  }

  private BitmapDrawable getCurrentStateBitmapDrawable()
  {
    return mPressedState ? mPressedStateInnerBitmap : mDefaultStateInnerBitmap;
  }

  public void setBounds(Rect bounds)
  {
    mOuterBitmap.setBounds(bounds);
  }

  public void setOpacity(int value)
  {
    mOpacity = value;

    mDefaultStateInnerBitmap.setAlpha(value);
    mPressedStateInnerBitmap.setAlpha(value);

    if (trackId == -1)
    {
      mOuterBitmap.setAlpha(value);
      mBoundsBoxBitmap.setAlpha(0);
    }
    else
    {
      mOuterBitmap.setAlpha(0);
      mBoundsBoxBitmap.setAlpha(value);
    }
  }

  public void setColor(int color)
  {
    mDefaultStateInnerBitmap.setTint(color);
    mBoundsBoxBitmap.setTint(color);
    mOuterBitmap.setTint(color);

    int newAlpha = (mPressedStateInnerBitmap.getAlpha() / 3) << 24;
    int colorWithoutAlpha = color & 0x00FFFFFF;
    mPressedStateInnerBitmap.setTint(newAlpha | colorWithoutAlpha);
  }

  public Rect getBounds()
  {
    return mOuterBitmap.getBounds();
  }

  private void setVirtBounds(Rect bounds)
  {
    mVirtBounds = bounds;
  }

  private void setOrigBounds(Rect bounds)
  {
    mOrigBounds = bounds;
  }

  private Rect getVirtBounds()
  {
    return mVirtBounds;
  }

  public int getWidth()
  {
    return mWidth;
  }

  public int getHeight()
  {
    return mHeight;
  }

  public int getTrackId()
  {
    return trackId;
  }
}
