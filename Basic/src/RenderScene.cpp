#include "RenderScene.h"

RenderScene::RenderScene(Vulkan* _vulkan, bool* _key, MousePos* _mousePos)
{
	vulkan = _vulkan;
	key = _key;
	mousePos = _mousePos;
	identity4(mView);
	vertexData = new VertexData;
	indexData = new IndexData;

	// Vertex Data
	vertexData->addData((float*)verticesPlane, sizeof(verticesPlane));
	vertexData->addData((float*)verticesStar, sizeof(verticesStar));
	vecf(&verticesCurve, &verticesCurveSize, 0.0f, 0.001f, 1001);
	vertexData->addData(verticesCurve, verticesCurveSize);
	
	// Index Data
	indexData->addData((uint16_t*)indicesPlane, sizeof(indicesPlane));
	indexData->addData((uint16_t*)indicesStar, sizeof(indicesStar));
	vecs(&indicesCurve, &indicesCurveSize, 0, 1001);
	indexData->addData(indicesCurve, indicesCurveSize);
	
	// Objects
	objectCount = 8;
	obj = new RenderObject*[objectCount];
	createDescriptorPool();
	obj[0] = new Square(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[1] = new Tacho(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[2] = new FlatPerlin2d(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[3] = new Star(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[4] = new FilledCircle(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[5] = new PerlinCircle(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[6] = new Wave(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	obj[7] = new Perlin1d(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	textOverlay = new TextOverlay(vulkan);
	txtObj = new TxtObj(vulkan, descriptorPool, textOverlay, &mView, key, mousePos, vertexData, indexData);
	graph = new Graph(vulkan, descriptorPool, nullptr, &mView, key, mousePos, vertexData, indexData);
	textOverlay->beginTextUpdate();
	textOverlay->addText("!!!***HalliHallo***!!!", 550.0f, 20.0f);
	textOverlay->endTextUpdate();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
}

RenderScene::~RenderScene() {}

void RenderScene::printMatrix(mat4 M, float x, float y)
{
	char str[32];
	float xOffset;
	for (unsigned int i = 0; i < 4; i++)
	{
		xOffset = x;
		for (unsigned int j = 0; j < 4; j++)
		{
			sprintf(str, "%6.2f", M[j][i]);
			textOverlay->addText(str, xOffset, y);
			xOffset += 125.0;
		}
		y += 30.0;
	}
}

void RenderScene::createVertexBuffer()
{
	vertexBuffer = new Buffer(vulkan);
	vertexBuffer->createDeviceLocalBuffer(vertexData->getData(), vertexData->getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void RenderScene::createIndexBuffer()
{
	indexBuffer = new Buffer(vulkan);
	indexBuffer->createDeviceLocalBuffer(indexData->getData(), indexData->getSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void RenderScene::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize[2] = {};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = (objectCount + 2) * 2;
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = 2;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes = poolSize;
	poolInfo.maxSets = objectCount + 2;

	if (vkCreateDescriptorPool(vulkan->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		std::cout << "Failed to create descriptor pool!" << std::endl;
		exit(1);
	}
}

void RenderScene::createCommandBuffers()
{
	commandBuffers = new VkCommandBuffer[vulkan->getSwapChainImagesCount()];

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkan->getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = vulkan->getSwapChainImagesCount();

	if (vkAllocateCommandBuffers(vulkan->getDevice(), &allocInfo, commandBuffers) != VK_SUCCESS) {
		std::cout << "failed to allocate command buffers!" << std::endl;
		exit(1);
	}

	for (uint32_t i = 0; i < vulkan->getSwapChainImagesCount(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
		{
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vulkan->getRenderPass();
			renderPassInfo.framebuffer = vulkan->getSwapChainFramebuffers()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vulkan->getSwapChainExtent();
			VkClearValue clearValues = {};
			clearValues.color = { {1.0f, 1.0f, 1.0f, 1.0f} };
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearValues;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				VkBuffer vB[] = { vertexBuffer->getBuffer() };
				vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

				for (uint32_t j = 0; j < objectCount; j++)
				{
					VkDeviceSize offsets[] = { obj[j]->getVertexOffset() };
					vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vB, offsets);
					vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obj[j]->getGraphicsPipeline());
					vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obj[j]->getPipelineLayout(), 0, 1, obj[j]->getDescriptorSetPtr(), 0, nullptr);
					vkCmdDrawIndexed(commandBuffers[i], obj[j]->getIndexCount(), 1, obj[j]->getFirstIndex(), 0, 0);
				}

				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, txtObj->getGraphicsPipeline());
				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, txtObj->getPipelineLayout(), 0, 1, txtObj->getDescriptorSetPtr(), 0, nullptr);
				VkDeviceSize offsets1 = 0;
				VkBuffer vB1[] = { txtObj->getTextOverlayVertexBuffer() };
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vB1, &offsets1);
				for (uint32_t j = 0; j < txtObj->getNumLetters(); j++)
				{
					vkCmdDraw(commandBuffers[i], 4, 1, j * 4, 0);
				}

				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graph->getGraphicsPipeline());
				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graph->getPipelineLayout(), 0, 1, graph->getDescriptorSetPtr(), 0, nullptr);
				offsets1 = 0;
				vB1[0] = graph->getVertexBuffer();
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vB1, &offsets1);
				vkCmdDraw(commandBuffers[i], graph->getVertexCount(), 1, 0, 0);
			}
			vkCmdEndRenderPass(commandBuffers[i]);
		}
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			std::cout << "failed to record command buffer!" << std::endl;
			exit(1);
		}
	}
}

void RenderScene::updateUniformBuffers()
{
	for (uint32_t i = 0; i < objectCount; i++)
	{
		obj[i]->updateUniformBuffer();
	}
	graph->updateUniformBuffer();
}

void RenderScene::camMotion()
{
	for (uint32_t i = 0; i < objectCount; i++) obj[i]->motion();

	//2d cam motion
	mat4 T, tmp;

	if (key[0x41] == true)
	{
		dup4(tmp, mView);
		getTrans4(T, 0.1f, 0.0f, 0.0f);
		mult4(mView, T, tmp);
	}

	if (key[0x44] == true)
	{
		dup4(tmp, mView);
		getTrans4(T, -0.1f, 0.0f, 0.0f);
		mult4(mView, T, tmp);
	}

	if (key[0x53] == true)
	{
		dup4(tmp, mView);
		getTrans4(T, 0.0f, -0.1f, 0.0f);
		mult4(mView, T, tmp);
	}

	if (key[0x57] == true)
	{
		dup4(tmp, mView);
		getTrans4(T, 0.0f, 0.1f, 0.0f);
		mult4(mView, T, tmp);
	}
}

void RenderScene::updateTextOverlay(uint32_t fps)
{
	char str[32];
	textOverlay->beginTextUpdate();
	sprintf(str, "FPS: %-4u", fps);
	textOverlay->addText(str, 5.0f, 5.0f);
	printMatrix(mView, 5.0, 35.0);
	textOverlay->endTextUpdate();
}

void RenderScene::drawFrame()
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vulkan->getDevice(), vulkan->getSwapChain(), std::numeric_limits<uint64_t>::max(), vulkan->getImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { vulkan->getImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	VkSemaphore signalSemaphores[] = { vulkan->getRenderFinishedSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vulkan->getQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		std::cout << "failed to submit draw command buffer!" << std::endl;
		exit(1);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { vulkan->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(vulkan->getQueue(), &presentInfo);

	vkQueueWaitIdle(vulkan->getQueue()); // Synchronisation der App mit der GPU (nicht notwendig)
}