/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, LLC
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <dtVoxel/voxelgrid.h>
#include <dtUtil/log.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/mathdefines.h>
#include <osgUtil/Optimizer>
#include <iostream>

namespace dtVoxel
{

   VoxelGrid::VoxelGrid()
      : mBlocksX(0)
      , mBlocksY(0)
      , mBlocksZ(0)
      , mNumBlocks(0)
      , mViewDistance(1000.0f)
      , mGridOffset()
      , mWSDimensions()
      , mBlockDimensions()
      , mCellDimensions()
      , mInitialized(false)
      , mTextureResolution()
      , mRootNode(new osg::Group())
      , mBlocks(NULL)
   {

   }

   VoxelGrid::~VoxelGrid()
   {
   }

   void VoxelGrid::Init(const osg::Vec3& grid_offset, const osg::Vec3& dimensions, const osg::Vec3& block_dimensions, const osg::Vec3& cellDimensions, const osg::Vec3i& textureResolution)
   {
      mGridOffset = grid_offset;
      mWSDimensions = dimensions;
      mBlockDimensions = block_dimensions;
      mCellDimensions = cellDimensions;
      mTextureResolution = textureResolution;

      mBlocksX = int(std::floor(mWSDimensions[0] / mBlockDimensions[0]));
      mBlocksY = int(std::floor(mWSDimensions[1] / mBlockDimensions[1]));
      mBlocksZ = int(std::floor(mWSDimensions[2] / mBlockDimensions[2]));

      mNumBlocks = mBlocksX * mBlocksY * mBlocksZ;

      mInitialized = true;
   }

   osg::BoundingBox VoxelGrid::ComputeWorldBounds(const osg::Vec3& pos)
   {
      osg::Vec3 bbMin;
      osg::Vec3 bbMax;

      osg::Vec3 clampMax = mGridOffset + mWSDimensions;
      
      
      for (int i = 0; i < 3; ++i)
      {
         bbMin[i] = pos[i] - mViewDistance;         
         dtUtil::Clamp(bbMin[i], mGridOffset[i], clampMax[i]);

         bbMax[i] = pos[i] + mViewDistance;
         dtUtil::Clamp(bbMax[i], mGridOffset[i], clampMax[i]);
      }

      return osg::BoundingBox(bbMin, bbMax);
   }


   void VoxelGrid::UpdateGrid(const osg::Vec3& newCameraPos)
   {
      //std::cout << "New Camera Pos ";

      //dtUtil::MatrixUtil::Print(newCameraPos);

      //std::cout << "Allocated Bounds " << std::endl;
      //dtUtil::MatrixUtil::Print(mAllocatedBounds._min);
      //dtUtil::MatrixUtil::Print(mAllocatedBounds._max);


      osg::BoundingBox newBounds = ComputeWorldBounds(newCameraPos);

      //std::cout << "New Bounds " << std::endl;
      //dtUtil::MatrixUtil::Print(newBounds._min);
      //dtUtil::MatrixUtil::Print(newBounds._max);

      
      osg::BoundingBox combinedBounds = mAllocatedBounds;

      combinedBounds.expandBy(newBounds);

      //std::cout << "combinedBounds " << std::endl;
      //dtUtil::MatrixUtil::Print(combinedBounds._min);
      //dtUtil::MatrixUtil::Print(combinedBounds._max);


      osg::Vec3 newDim;
      newDim[0] = combinedBounds.xMax() - combinedBounds.xMin();
      newDim[1] = combinedBounds.yMax() - combinedBounds.yMin();
      newDim[2] = combinedBounds.zMax() - combinedBounds.zMin();

      int blocksX = int(std::floor(newDim.x() / mBlockDimensions.x()));
      int blocksY = int(std::floor(newDim.y() / mBlockDimensions.y()));
      int blocksZ = int(std::floor(newDim.z() / mBlockDimensions.z()));

      //std::cout << "Blocks X " << blocksX << " Blocks Y " << blocksY << " Blocks Z " << blocksZ << std::endl;

      osg::Vec3 bboffset = combinedBounds._min - mGridOffset;
      int startX = int(std::floor(bboffset.x() / mBlockDimensions.x()));
      int startY = int(std::floor(bboffset.y() / mBlockDimensions.y()));
      int startZ = int(std::floor(bboffset.z() / mBlockDimensions.z()));

      //std::cout << "start X " << startX << " start Y " << startY << " start Z " << startZ << std::endl;

      int endX = startX + blocksX;
      int endY = startY + blocksY;
      int endZ = startZ + blocksZ;

      //std::cout << "end X " << endX << " end Y " << endY << " end Z " << endZ << std::endl;

      for (int z = startZ; z < endZ; z++)
      {
         for (int y = startY; y < endY; y++)
         {
            for (int x = startX; x < endX; x++)
            {
               VoxelBlock* curBlock = GetBlockFromIndex(x, y, z);

               if (curBlock != nullptr)
               {
                  osg::Vec3 centerPos = GetCenterOfBlock(x, y, z);

                  bool posInOldBounds = mAllocatedBounds.contains(centerPos);
                  bool posInNewBounds = newBounds.contains(centerPos);

                  if (posInNewBounds)
                  {
                     //allocate
                     if (!curBlock->GetEmpty() && !curBlock->IsAllocated())
                     {
                        curBlock->Allocate(*mVoxelActor, mTextureResolution);
                        mRootNode->addChild(curBlock->GetOSGNode());
                     }

                  }
                  else if (posInOldBounds)
                  {
                     if (curBlock->IsAllocated())
                     {
                        //deallocate
                        curBlock->DeAllocate();
                        mRootNode->removeChild(curBlock->GetOSGNode());
                     }
                  }

               }
               else
               {
                  LOG_ERROR("Invalid block dimensions calculated when updating grid.");
               }
            }
         }
      }

      mAllocatedBounds = newBounds;
   }


   void VoxelGrid::CreateGridFromActor(const osg::Vec3& pos, VoxelActor& voxelActor)
   {
      if (!mInitialized)
      {
         LOG_ERROR("VoxelGrid has not been initialized.");
         return;
      }

      mAllocatedBounds = ComputeWorldBounds(pos);
      
      mVoxelActor = &voxelActor;
      
      /*osg::MatrixTransform* mt = new osg::MatrixTransform();
      osg::Matrix mat;
      mat.makeScale(1.0f, 1.0f, 1.0f);
      mt->setMatrix(mat);

      mRootNode = mt;*/

      mBlocks = new VoxelBlock[mNumBlocks];

      std::cout << std::endl << "Creating Voxel Grid with " << mNumBlocks << " Voxel Blocks" << std::endl;

      dtCore::RefPtr<osg::Group> currentGroup = NULL;
      
      for (int z = 0; z < mBlocksZ; z++)
      {
         for (int y = 0; y < mBlocksY; y++)         
         {
            for (int x = 0; x < mBlocksX; x++)
            {
               VoxelBlock* curBlock = GetBlockFromIndex(x, y, z);

               osg::Vec3 offsetFrom = mGridOffset;
               offsetFrom[0] += x * mBlockDimensions[0];
               offsetFrom[1] += y * mBlockDimensions[1];
               offsetFrom[2] += z * mBlockDimensions[2];

               osg::Vec3 offsetTo = offsetFrom + mBlockDimensions;

               curBlock->Init(mBlockDimensions, offsetFrom, mCellDimensions);

               //check if block is within view distance

               osg::BoundingBox bb(offsetFrom, offsetTo);
               openvdb::GridBase::Ptr vdbGrid = mVoxelActor->CollideWithAABB(bb);

               if (vdbGrid != NULL && !vdbGrid->empty())
               {
                  //only allocate this block if it is within our visual bounds
                  if (mAllocatedBounds.contains(GetCenterOfBlock(x, y, z)))
                  {
                     //allocate this block
                     curBlock->Allocate(*mVoxelActor, mTextureResolution);

                     //todo- spatially subdivide
                     mRootNode->addChild(curBlock->GetOSGNode());
                  }
                  
                  curBlock->SetEmpty(false);
               }
            }
            std::cout << ".";
         }
      }

      std::cout << std::endl << "Done Creating Voxel Grid" << std::endl;

      std::cout << std::endl << "Optimizing Grid" << std::endl;
      osgUtil::Optimizer opt;
      opt.optimize(mRootNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);
      std::cout << std::endl << "Done Optimizing" << std::endl;
   }

   VoxelBlock* VoxelGrid::GetBlockFromIndex(int x, int y, int z)
   {
      VoxelBlock* block = NULL;

      int index = (z * mBlocksY * mBlocksX) + (y * mBlocksX) + x;
      
      if (index >= 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }
      else
      {
         LOG_ERROR("Block index out of bounds.");
      }
      
      return block;
   }

   osg::Vec3 VoxelGrid::GetCenterOfBlock(int x, int y, int z)
   {
      float blockX = x * mBlockDimensions.x();
      float blockY = y * mBlockDimensions.y();
      float blockZ = z * mBlockDimensions.z();

      osg::Vec3 pos(blockX, blockY, blockZ);

      osg::Vec3 halfBlock = mBlockDimensions * 0.5;

      pos += halfBlock;
      pos += mGridOffset;
      
      return pos;
   }


   VoxelBlock* VoxelGrid::GetBlockFromIndex(int index)
   {
      VoxelBlock* block = NULL;

      if (index >= 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }
      else
      {
         LOG_ERROR("Block index out of bounds.");
      }

      return block;
   }

   VoxelBlock* VoxelGrid::GetBlockFromPos(const osg::Vec3& pos)
   {
      VoxelBlock* block = NULL;
      osg::Vec3 offset = pos - mGridOffset;
      int cellX = int(std::floor(pos[0] / mBlockDimensions[0]));
      int cellY = int(std::floor(pos[1] / mBlockDimensions[1]));
      int cellZ = int(std::floor(pos[2] / mBlockDimensions[2]));

      int index = (cellZ * mBlocksY * mBlocksX) + (cellY * mBlocksX) + cellX;

      if (index > 0 && index < mNumBlocks)
      {
         block = &mBlocks[index];
      }

      return block;
   }

   const osg::Vec3i& VoxelGrid::GetTextureResolution() const
   {
      return mTextureResolution;
   }

   osg::Node* VoxelGrid::GetOSGNode()
   {
      return mRootNode.get();
   }

   const osg::Node* VoxelGrid::GetOSGNode() const
   {
      return mRootNode.get();
   }

} /* namespace dtVoxel */